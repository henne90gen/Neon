#pragma once

#include <optional>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "Module.h"
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
    explicit FileCodeProvider(const Module *program) : fileName(program->fileName) {}

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

class Lexer {
  public:
    explicit Lexer(CodeProvider *codeProvider, Module *program, bool verbose = false)
        : codeProvider(codeProvider), program(program), verbose(verbose){};

    Token getToken();

  private:
    std::string currentWord;
    CodeProvider *codeProvider;
    Module *program;
    bool verbose;

    Token _getToken();

    std::optional<Token> matchRegex(const std::string &regex, Token::TokenType tokenType);
    std::optional<Token> matchOneCharacterToken();
    std::optional<Token> matchTwoCharToken();
    std::optional<Token> matchWordToken();
};
