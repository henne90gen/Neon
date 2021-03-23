#include "Linker.h"

#include <cstdlib>
#include <iostream>

#if WIN32
void setenv(const std::string &key, const std::string &value) {
    const auto envStr = key + "=" + value;
    putenv(envStr.c_str());
}

std::string Linker::getLinkerCommand() {
    std::string hostArch = "x64";
    std::string targetArch = "x64";
    std::string windowsSdkDir = R"(C:\Program Files (x86)\Windows Kits\10\)";
    std::string windowsSdkVersion = "10.0.18362.0";
    std::string vsInstallDir = R"(C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\)";
    std::string vcInstallDir = vsInstallDir + "VC\\";
    std::string vcToolsVersion = "14.28.29910";
    std::string vcToolsInstallDir = vcInstallDir + "Tools\\MSVC\\" + vcToolsVersion + "\\";

    // setup path so that we can find the linker
    const auto pathBefore = std::string(getenv("PATH"));
    std::string newPath;
    newPath += pathBefore + ";";
    newPath += vcToolsInstallDir + "bin\\Host" + hostArch + "\\" + targetArch + ";";
    setenv("PATH", newPath);

    std::string s = "link";

    // remove logo from output
    s += " /NOLOGO";

    // specify the search directories for libraries to link against
    // contains libcmt.lib
    s += " /LIBPATH:\"" + vcToolsInstallDir + "lib\\" + targetArch + "\"";
    // contains transitive dependecies of libucrt.lib
    s += " /LIBPATH:\"" + windowsSdkDir + "lib\\" + windowsSdkVersion + "\\um\\" + targetArch + "\"";
    // contains libucrt.lib (universal c runtime)
    s += " /LIBPATH:\"" + windowsSdkDir + "lib\\" + windowsSdkVersion + "\\ucrt\\" + targetArch + "\"";
    // contains the Neon standard library
    s += " /LIBPATH:\"" + buildEnv->buildDirectory + "\"";

    // link against the c runtime library
    s += " /DEFAULTLIB:\"libucrt\"";
    s += " /DEFAULTLIB:\"libcmt\"";
    s += " /DEFAULTLIB:\"libvcruntime\"";
    s += " /DEFAULTLIB:\"NeonStd\"";

    // don't link against this version of the c runtime library
    s += " /NODEFAULTLIB:\"msvcrtd\"";

    // ignore some linker warning
    s += " /IGNORE:4217";

    // define the name of the final executable fille
    s += " /OUT:\"" + buildEnv->buildDirectory + program->executableFileName() + "\"";

    // specify the object file to link
    s += " " + buildEnv->buildDirectory + program->objectFileName();

    return s;
}
#else
std::string Linker::getLinkerCommand() {
    // TODO find out what these options do:
    //  -pie --eh-frame-hdr

#if 0
    std::string s = "ld";

    if (log.getLogLevel() == Logger::DEBUG_) {
        s += " --verbose";
    }

    s += " -m elf_x86_64";

    // specify which dynamic linker should be used at runtime for dynamically linked libraries
    s += " -dynamic-linker /lib64/ld-linux-x86-64.so.2";

    // specify output file name
    s += " -o " + buildEnv->buildDirectory + program->executableFileName();

    // TODO find a robust way to determine the c runtime library location (crt1.o, crti.o, crtn.o)
    // additional object files that have to be linked (they contain the actual entry point and some setup stuff)
    // c runtime library
    s += " /lib64/crt1.o";
    s += " /lib64/crti.o";
    s += " /lib64/crtn.o";

    // search directory for c standard library and math library
    s += " -L/usr/lib/gcc/x86_64-pc-linux-gnu/9.2.0/";

    // search directory for Neon standard library
    s += " -L" + buildEnv->buildDirectory;

    // specify the object file to link
    s += " " + buildEnv->buildDirectory + program->objectFileName();

    // c: c standard library, m: math library
    s += " -lc -lm";

    // Neon standard library
    s += " -lNeonStd";

    return s;
#else
#if 1
    std::string s = "\"" + buildEnv->buildDirectory + "crt/bin/ld.musl-clang\"";
#else
    std::string s = "ld";
#endif
    if (log.getLogLevel() == Logger::DEBUG_) {
        s += " --verbose";
    }

    s += " --sysroot=" + buildEnv->buildDirectory + "crt";

    //    s += " -m elf_x86_64";

    // specify which dynamic linker should be used at runtime for dynamically linked libraries
    //    s += " -dynamic-linker /lib64/ld-linux-x86-64.so.2";
    // link everything statically
    s += " -static";

    // specify output file name
    s += " -o " + buildEnv->buildDirectory + program->executableFileName();

    // TODO find a robust way to determine the c runtime library location (crt1.o, crti.o, crtn.o)
    // additional object files that have to be linked (they contain the actual entry point and some setup stuff)
    // c runtime library
    s += " " + buildEnv->buildDirectory + "crt/lib/crt1.o";
    s += " " + buildEnv->buildDirectory + "crt/lib/crti.o";
    s += " " + buildEnv->buildDirectory + "crt/lib/crtn.o";

    // search directory for c standard library and math library
    s += " -L-user-start";
    s += " -L" + buildEnv->buildDirectory + "crt/lib/";
    s += " -L-user-end";
    s += " -L" + buildEnv->buildDirectory + "crt/lib/";

    // specify the object file to link
    s += " " + buildEnv->buildDirectory + program->objectFileName();

    // c: c standard library, m: math library
    s += " -lc -lm";

    // Neon standard library
    s += " " + buildEnv->buildDirectory + "libNeonStd.a";
    return s;
#endif
}
#endif

bool Linker::link() {
    std::string linkerCommand = getLinkerCommand();

    log.debug("Calling linker with the following command:\n" + linkerCommand);

    // TODO capture stdout and stderr. only print to console when verbose==true
    const char *command = linkerCommand.c_str();
    int statusCode = system(command);

    log.debug("Finished linking.");

    return statusCode != 0;
}
