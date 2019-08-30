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

class Lexer {
public:
  Lexer(CodeProvider *codeProvider) : codeProvider(codeProvider){};

  Token getToken();

private:
  std::string currentWord;
  CodeProvider *codeProvider;

  Token matchOneCharacterToken();
};
