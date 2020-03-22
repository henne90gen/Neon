#pragma once

#include <optional>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "../Module.h"
#include "Token.h"

class CodeProvider {
  public:
    virtual std::optional<std::string> getMoreCode() = 0;
};

class FileCodeProvider : public CodeProvider {
  public:
    explicit FileCodeProvider(std::string absoluteFilePath) : fileName(std::move(absoluteFilePath)) {}

    std::optional<std::string> getMoreCode() override;

  private:
    const std::string fileName;
    bool fileHasBeenRead = false;
    std::vector<std::string> lines = {};
};

class StringCodeProvider : public CodeProvider {
  public:
    explicit StringCodeProvider(std::vector<std::string> lines, bool addLineBreaks)
        : lines(std::move(lines)), addLineBreaks(addLineBreaks) {}

    std::optional<std::string> getMoreCode() override;

  private:
    std::vector<std::string> lines = {};
    bool addLineBreaks = false;
};

class ByteCodeProvider : public CodeProvider {
  public:
    explicit ByteCodeProvider(const char *data, const long size) : data(data), size(size) {}

    std::optional<std::string> getMoreCode() override;

  private:
    const char *data;
    long size;
};

class Lexer {
  public:
    explicit Lexer(CodeProvider *codeProvider, bool verbose = false) : codeProvider(codeProvider), verbose(verbose){};

    Token getToken();

  private:
    std::string currentWord;
    CodeProvider *codeProvider;
    bool verbose;

    std::optional<Token> matchRegex(const std::string &regex, Token::TokenType tokenType);
    std::optional<Token> matchOneCharacterToken();
    std::optional<Token> matchTwoCharToken();
    std::optional<Token> matchWordToken();
};
