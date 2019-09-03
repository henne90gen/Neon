#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "Token.h"

class CodeProvider {
public:
    virtual std::optional<std::string> getMoreCode() = 0;
};

class StdInCodeProvider : public CodeProvider {
public:
    std::optional<std::string> getMoreCode() override;
};

class FileCodeProvider : public CodeProvider {
public:
    explicit FileCodeProvider(std::string fileName) : fileName(std::move(fileName)) {}

    std::optional<std::string> getMoreCode() override;

private:
    const std::string fileName;
    bool fileHasBeenRead = false;
    std::vector<std::string> lines;
};

class StringCodeProvider : public CodeProvider {
public:
    explicit StringCodeProvider(std::vector<std::string> lines) : lines(std::move(lines)) {}

    std::optional<std::string> getMoreCode() override;

private:
    std::vector<std::string> lines;
};

class Lexer {
public:
    explicit Lexer(CodeProvider *codeProvider, bool verbose = false)
            : codeProvider(codeProvider), verbose(verbose) {};

    Token getToken();

private:
    std::string currentWord;
    CodeProvider *codeProvider;
    bool verbose;

    std::optional<Token> matchOneCharacterToken();

    std::optional<Token> matchTwoCharToken();

    std::optional<Token> matchWordToken();
};
