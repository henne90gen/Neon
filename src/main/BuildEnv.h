#pragma once

#include <filesystem>
#include <string>

struct BuildEnv {
    std::string buildDirectory = "./neon-build/";

    explicit BuildEnv() { createBuildDir(); }
    explicit BuildEnv(const std::string &buildDir) : buildDirectory(buildDir) {
        if (buildDirectory.back() != '/') {
            buildDirectory += '/';
        }
        createBuildDir();
    }

    void createBuildDir() {
        if (std::filesystem::exists(buildDirectory)) {
            return;
        }

        std::filesystem::create_directory(buildDirectory);
    }
};
