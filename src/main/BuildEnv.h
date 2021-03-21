#pragma once

#include <filesystem>
#include <string>
#include <utility>

struct BuildEnv {
    std::string buildDirectory = "./neon-build/";

    explicit BuildEnv() { createBuildDir(); }
    explicit BuildEnv(std::string buildDir) : buildDirectory(std::move(buildDir)) {
        if (buildDirectory.back() != '/') {
            buildDirectory += '/';
        }
        createBuildDir();
    }

    void createBuildDir() const {
        if (std::filesystem::exists(buildDirectory)) {
            return;
        }

        std::filesystem::create_directory(buildDirectory);
    }
};
