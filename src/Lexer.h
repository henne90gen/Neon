#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Token.h"

class CodeProvider {
public:
    virtual std::optional<std::string> getMoreCode() = 0;
};

class StdInCodeProvider : public CodeProvider {
public:
    virtual std::optional<std::string> getMoreCode() override;
};

class FileCodeProvider : public CodeProvider {
public:
    FileCodeProvider(std::string fileName) : fileName(fileName) {}

    virtual std::optional<std::string> getMoreCode() override;

private:
    std::string fileName;
    bool fileHasBeenRead = false;
    std::vector<std::string> lines;
};

class StringCodeProvider : public CodeProvider {
public:
    StringCodeProvider(std::vector<std::string> &lines) : lines(lines) {}

    virtual std::optional<std::string> getMoreCode() override;

private:
    std::vector<std::string> lines;
};

class Lexer {
public:
    Lexer(CodeProvider *codeProvider, bool verbose = false)
            : codeProvider(codeProvider), verbose(verbose) {};

    Token getToken();

private:
    std::string currentWord;
    CodeProvider *codeProvider;
    bool verbose;

    Token matchOneCharacterToken();
};
