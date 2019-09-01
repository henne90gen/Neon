#include "Lexer.h"

#include <fstream>
#include <iostream>
#include <regex>

std::optional<std::string> StdInCodeProvider::getMoreCode() {
  std::string result = "";
  std::cin >> result;
  if (result.empty()) {
    return {};
  }
  return std::optional(result);
}

std::optional<std::string> FileCodeProvider::getMoreCode() {
  if (!fileHasBeenRead) {
    fileHasBeenRead = true;
    std::ifstream infile(fileName);
    std::string line;
    while (std::getline(infile, line)) {
      lines.push_back(line);
    }
  }

  if (lines.empty()) {
    return {};
  }

  std::string result = lines.front();
  lines.erase(lines.begin());
  return std::optional(result);
}

void removeLeadingWhitespace(std::string &str) {
  while (!str.empty() && str[0] == ' ') {
    str = str.substr(1, str.length());
  }
}

Token Lexer::getToken() {
  while (true) {
    if (currentWord.empty()) {
      auto optionalCode = codeProvider->getMoreCode();
      if (!optionalCode) {
        break;
      }
      currentWord = optionalCode.value();
      if (currentWord.empty()) {
        continue;
      }
    }

    removeLeadingWhitespace(currentWord);
    if (verbose) {
      std::cout << "Current word: '" << currentWord << "'" << std::endl;
    }

    std::regex floatRegex("^[0-9]+.[0-9]+");
    auto itr = std::sregex_iterator(currentWord.begin(), currentWord.end(),
                                    floatRegex);
    if (itr != std::sregex_iterator()) {
      auto content = (*itr).str();
      currentWord =
          currentWord.substr(content.length(), currentWord.length() - 1);
      return {Token::FLOAT_LIT, content};
    }

    std::regex intRegex("^[0-9]+");
    itr =
        std::sregex_iterator(currentWord.begin(), currentWord.end(), intRegex);
    if (itr != std::sregex_iterator()) {
      auto content = (*itr).str();
      currentWord =
          currentWord.substr(content.length(), currentWord.length() - 1);
      return {Token::INT_LIT, content};
    }

    if (currentWord.find("true") == 0) {
      currentWord = currentWord.substr(4, currentWord.length() - 1);
      return {Token::TRUE, "true"};
    }

    if (currentWord.find("false") == 0) {
      currentWord = currentWord.substr(5, currentWord.length() - 1);
      return {Token::FALSE, "false"};
    }

    if (currentWord.find("not") == 0) {
      currentWord = currentWord.substr(3, currentWord.length() - 1);
      return {Token::NOT, "not"};
    }

    auto oneCharToken = matchOneCharacterToken();
    if (oneCharToken.type != Token::END_OF_FILE) {
      currentWord = currentWord.substr(1, currentWord.length() - 1);
      return oneCharToken;
    }
  }

  return {Token::END_OF_FILE, ""};
}

Token Lexer::matchOneCharacterToken() {
  char firstChar = currentWord[0];
  if (firstChar == '(') {
    return {Token::LEFT_PARAN, "("};
  } else if (firstChar == ')') {
    return {Token::RIGHT_PARAN, ")"};
  } else if (firstChar == '+') {
    return {Token::PLUS, "+"};
  } else if (firstChar == '-') {
    return {Token::MINUS, "-"};
  } else if (firstChar == '*') {
    return {Token::STAR, "*"};
  } else if (firstChar == '/') {
    return {Token::DIV, "/"};
  } else if (firstChar == ';') {
    return {Token::SEMICOLON, ";"};
  }
  return {Token::END_OF_FILE, ""};
}
