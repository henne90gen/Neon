#pragma once

#include "Token.h"
#include <string>
#include <vector>

class CodeProvider {
public:
  virtual std::string getMoreCode() = 0;
};

class StdInCodeProvider : public CodeProvider {
public:
  virtual std::string getMoreCode();
};

class FileCodeProvider : public CodeProvider {
public:
  FileCodeProvider(std::string fileName) : fileName(fileName) {}
  virtual std::string getMoreCode();

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
