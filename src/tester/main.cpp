#include <filesystem>
#include <iostream>
#include <string>

#include <BuildEnv.h>
#include <Linker.h>
#include <Program.h>
#include <compiler/Compiler.h>

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

struct TestResult {
    int exitCode = 0;
    std::chrono::nanoseconds compileTime = {};
    std::chrono::nanoseconds runTime = {};

    [[nodiscard]] bool success() const { return exitCode == 0; }
    [[nodiscard]] double compileTimeMillis() const { return static_cast<double>(compileTime.count()) / 1.0e+6; }
    [[nodiscard]] double runTimeMillis() const { return static_cast<double>(runTime.count()) / 1.0e+6; }
};

TestResult compileAndRun(const std::string &path, const bool verbose) {
    auto program = new Program(path);
    auto buildEnv = new BuildEnv();

    auto start = std::chrono::high_resolution_clock::now();
    auto compiler = Compiler(program, buildEnv, verbose);
    if (compiler.run()) {
        return {
              .exitCode = -1,
        };
    }

    auto linker = Linker(program, buildEnv, verbose);
    if (linker.link()) {
        return {
              .exitCode = -1,
        };
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto compileTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    start = std::chrono::high_resolution_clock::now();
    int exitCode = std::system((buildEnv->buildDirectory + program->name).c_str());
    end = std::chrono::high_resolution_clock::now();

    auto runTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    return {
          .exitCode = WEXITSTATUS(exitCode),
          .compileTime = compileTimeNs,
          .runTime = runTime,
    };
}

int main() {
    std::string testDirectory = "tests/";
    bool verbose = false;

    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    bool success = true;

    int totalNumTests = 0;
    int successfulTests = 0;
    double compileTimeTotalMillis = 0;
    double runTimeTotalMillis = 0;
    for (const auto &path : std::filesystem::recursive_directory_iterator(testDirectory)) {
        if (path.is_directory()) {
            continue;
        }
        if (!endsWith(path.path().string(), "_test.ne")) {
            continue;
        }
        totalNumTests++;

        const TestResult &result = compileAndRun(path.path().string(), verbose);
        if (!result.success()) {
            std::cout << "FAILURE";
            success = false;
        } else {
            std::cout << "SUCCESS";
            successfulTests++;
        }

        compileTimeTotalMillis += result.compileTimeMillis();
        runTimeTotalMillis += result.runTimeMillis();
        std::cout << " (compile: " << std::setw(7) << result.compileTimeMillis() << "ms, run: " << std::setw(7)
                  << result.runTimeMillis() << "ms, exitCode: " << result.exitCode << "): " << path << std::endl;
    }

    int exitCode = 0;
    if (!success) {
        exitCode = 1;
    }
    std::cout << std::endl
              << "RESULTS (compile: " << std::setw(7) << compileTimeTotalMillis << "ms, run: " << std::setw(7)
              << runTimeTotalMillis << "ms, exitCode: " << exitCode << "): " << successfulTests << "/" << totalNumTests
              << " tests successful" << std::endl;

    return exitCode;
}
