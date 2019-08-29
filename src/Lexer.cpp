#include "Lexer.h"

#include <fstream>
#include <iostream>
#include <regex>

std::string StdInCodeProvider::getMoreCode() {
  std::string result = "";
  std::cin >> result;
  return result;
}

std::string FileCodeProvider::getMoreCode() {
  if (!fileHasBeenRead) {
    fileHasBeenRead = true;
    std::ifstream infile(fileName);
    std::string line;
    while (std::getline(infile, line)) {
      lines.push_back(line);
    }
  }

  if (lines.empty()) {
    return "";
  }

  std::string result = lines.front();
  lines.erase(lines.begin());
  return result;
}

void removeLeadingWhitespace(std::string &str) {
  while (str != "" && str[0] == ' ') {
    str = str.substr(1, str.length());
  }
}

Token Lexer::getToken() {
  while (true) {
    if (currentWord.empty()) {
      currentWord = codeProvider->getMoreCode();
    }

    removeLeadingWhitespace(currentWord);
    // std::cout << "Current word: " << currentWord << std::endl;

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

    auto oneCharToken = matchOneCharacterToken();
    if (oneCharToken.type != Token::END_OF_FILE) {
      currentWord = currentWord.substr(1, currentWord.length() - 1);
      return oneCharToken;
    }

    if (currentWord == "") {
      break;
    }
    std::cout << "Could not match: " << currentWord << std::endl;
    currentWord = "";
  }
  return {Token::END_OF_FILE, ""};
}

Token Lexer::matchOneCharacterToken() {
  if (currentWord.substr(0, 1) == "(") {
    return {Token::LEFT_PARAN, "("};
  } else if (currentWord.substr(0, 1) == ")") {
    return {Token::RIGHT_PARAN, ")"};
  } else if (currentWord.substr(0, 1) == "+") {
    return {Token::PLUS, "+"};
  } else if (currentWord.substr(0, 1) == "-") {
    return {Token::MINUS, "-"};
  } else if (currentWord.substr(0, 1) == "*") {
    return {Token::STAR, "*"};
  } else if (currentWord.substr(0, 1) == "/") {
    return {Token::DIV, "/"};
  }
  return {Token::END_OF_FILE, ""};
}
