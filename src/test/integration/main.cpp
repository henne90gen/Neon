#include <filesystem>
#include <iostream>
#include <string>

#include <BuildEnv.h>
#include <Linker.h>
#include <Program.h>
#include <compiler/Compiler.h>
#include <compiler/Logger.h>
#include <util/Timing.h>

struct CmdArguments {
    std::string testDirectory = "tests";
    std::string regex = {};
    bool verbose = false;
    bool noColor = false;
};

struct TestResult {
    int exitCode = 0;
    std::chrono::nanoseconds compileTime = {};
    std::chrono::nanoseconds linkTime = {};
    std::chrono::nanoseconds runTime = {};

    [[nodiscard]] bool success() const { return exitCode == 0; }
    [[nodiscard]] double compileTimeMillis() const { return static_cast<double>(compileTime.count()) / 1.0e+6; }
    [[nodiscard]] double linkTimeMillis() const { return static_cast<double>(linkTime.count()) / 1.0e+6; }
    [[nodiscard]] double runTimeMillis() const { return static_cast<double>(runTime.count()) / 1.0e+6; }
};

bool endsWith(const std::string &str, const std::string &pattern) {
    if (str.size() < pattern.size()) {
        return false;
    }
    for (int i = 0; i < pattern.size(); i++) {
        if (pattern[i] != str[str.size() - pattern.size() + i]) {
            return false;
        }
    }
    return true;
}

int runTest(const BuildEnv *buildEnv, const Program *program, TimeKeeper &timeKeeper) {
    auto timer = Timer(timeKeeper, "run");
    std::string executable = buildEnv->buildDirectory + program->executableFileName();
#if WIN32
    if (executable.starts_with("./")) {
        executable = executable.substr(2);
        for (char &c : executable) {
            if (c == '/') {
                c = '\\';
            }
        }
    }
#endif
    return std::system(executable.c_str());
}

TestResult compileAndRun(const std::string &path, const Logger &logger) {
    auto timeKeeper = TimeKeeper();
    auto program = new Program(path);
    auto buildEnv = new BuildEnv();

    {
        auto timer = Timer(timeKeeper, "compile");
        auto compiler = Compiler(program, buildEnv, logger);
        if (compiler.run()) {
            return {
                  .exitCode = -1,
            };
        }
    }

    {
        auto timer = Timer(timeKeeper, "link");
        auto linker = Linker(program, buildEnv, logger);
        if (linker.link()) {
            return {
                  .exitCode = -1,
            };
        }
    }

    int exitCode = runTest(buildEnv, program, timeKeeper);

#ifdef WIN32
    return {
          .exitCode = exitCode,
          .compileTime = timeKeeper.get("compile"),
          .linkTime = timeKeeper.get("link"),
          .runTime = timeKeeper.get("run"),
    };
#else
    return {
          .exitCode = WEXITSTATUS(exitCode),
          .compileTime = timeKeeper.get("compile"),
          .linkTime = timeKeeper.get("link"),
          .runTime = timeKeeper.get("run"),
    };
#endif
}

std::vector<std::filesystem::path> collectTests(const CmdArguments &args) {
    std::vector<std::filesystem::path> results = {};
    if (!std::filesystem::exists(args.testDirectory)) {
        std::cout << "Could not find test directory '" << args.testDirectory << "'" << std::endl;
        return results;
    }
    std::regex re(args.regex);

    for (const auto &path : std::filesystem::recursive_directory_iterator(args.testDirectory)) {
        if (path.is_directory()) {
            continue;
        }
        const std::string &pathStr = path.path().string();
        if (!endsWith(pathStr, "_test.ne")) {
            continue;
        }
        if (!args.regex.empty()) {
            auto itr = std::sregex_iterator(pathStr.begin(), pathStr.end(), re);
            if (itr == std::sregex_iterator()) {
                continue;
            }
        }

        results.push_back(path.path());
    }

    std::sort(results.begin(), results.end(), [](const std::filesystem::path &a, const std::filesystem::path &b) {
        const std::string &aStr = a.string();
        const std::string &bStr = b.string();
        return std::lexicographical_compare(aStr.begin(), aStr.end(), bStr.begin(), bStr.end());
    });

    return results;
}

CmdArguments parseArgs(int argc, char **argv) {
    CmdArguments result = {};
    if (argc == 1) {
        return result;
    }

    for (int i = 1; i < argc; i++) {
        const std::string &argument = std::string(argv[i]);
        std::cout << argument << std::endl;
        if (argument == "-d" || argument == "--directory") {
            if (i + 1 < argc) {
                result.testDirectory = std::string(argv[i + 1]);
                i++;
                continue;
            } else {
                std::cout << "expected directory, but there were no more arguments" << std::endl;
                continue;
            }
        } else if (argument == "-v" || argument == "--verbose") {
            result.verbose = true;
            continue;
        } else if (argument == "--plain") {
            result.noColor = true;
            continue;
        } else if (argument == "-r" || argument == "--regex") {
            if (i + 1 < argc) {
                result.regex = std::string(argv[i + 1]);
                i++;
                continue;
            } else {
                std::cout << "expected regular expression, but there were no more arguments" << std::endl;
                continue;
            }
        }
    }

    return result;
}

std::string addResultColor(bool success, bool bright) {
    if (bright) {
        if (success) {
            return "\u001b[92m";
        }
        return "\u001b[91m";
    }

    if (success) {
        return "\u001b[32m";
    }
    return "\u001b[31m";
}

int main(int argc, char **argv) {
    auto args = parseArgs(argc, argv);

    Logger logger = {};
    if (args.verbose) {
        logger.setLogLevel(Logger::LogLevel::DEBUG_);
    }
    logger.setColorEnabled(!args.noColor);

    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    bool success = true;

    int totalNumTests = 0;
    int successfulTests = 0;
    double compileTimeTotalMillis = 0;
    double runTimeTotalMillis = 0;
    double linkTimeTotalMillis = 0;

    std::vector<std::filesystem::path> tests = collectTests(args);
    for (const auto &path : tests) {
        totalNumTests++;

        const TestResult &result = compileAndRun(path.string(), logger);
        if (!result.success()) {
            std::cout << addResultColor(false, false) << "FAILURE";
            success = false;
        } else {
            std::cout << addResultColor(true, false) << "SUCCESS";
            successfulTests++;
        }
        std::cout << "\u001b[0m";

        compileTimeTotalMillis += result.compileTimeMillis();
        linkTimeTotalMillis += result.linkTimeMillis();
        runTimeTotalMillis += result.runTimeMillis();
        std::cout << " (compile: " << std::setw(7) << result.compileTimeMillis() << "ms, link: " << std::setw(7)
                  << result.linkTimeMillis() << "ms, run: " << std::setw(7) << result.runTimeMillis()
                  << "ms, exitCode: " << std::setw(2) << result.exitCode << "): " << path << std::endl;
    }

    int exitCode = 0;
    if (!success) {
        exitCode = 1;
    }

    std::cout << std::endl
              << addResultColor(successfulTests == totalNumTests, true) << "RESULTS (compile: " << std::setw(7)
              << compileTimeTotalMillis << "ms, link: " << std::setw(7) << linkTimeTotalMillis
              << "ms, run: " << std::setw(7) << runTimeTotalMillis << "ms, exitCode: " << std::setw(2) << exitCode
              << "): " << successfulTests << "/" << totalNumTests << " tests successful"
              << "\u001b[0m" << std::endl;

    return exitCode;
}
