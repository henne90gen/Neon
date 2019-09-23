#include "Lexer.h"

#include <fstream>
#include <iostream>
#include <optional>
#include <regex>

std::optional<std::string> StdInCodeProvider::getMoreCode() {
    std::string result;
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
        if (!infile.good()) {
            std::cerr << "Could not read file '" << fileName << "'." << std::endl;
        }
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

std::optional<std::string> StringCodeProvider::getMoreCode() {
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
    std::string previousWord = currentWord;
    while (true) {
        if (currentWord.empty()) {
            auto optionalCode = codeProvider->getMoreCode();
            if (!optionalCode.has_value()) {
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

        std::regex floatRegex("^[0-9]+\\.[0-9]+");
        auto itr = std::sregex_iterator(currentWord.begin(), currentWord.end(), floatRegex);
        if (itr != std::sregex_iterator()) {
            auto content = static_cast<std::string>((*itr).str());
            currentWord = currentWord.substr(content.length(), currentWord.length() - 1);
            return {Token::FLOAT, content};
        }

        std::regex intRegex("^[0-9]+");
        itr = std::sregex_iterator(currentWord.begin(), currentWord.end(), intRegex);
        if (itr != std::sregex_iterator()) {
            auto content = static_cast<std::string>((*itr).str());
            currentWord = currentWord.substr(content.length(), currentWord.length() - 1);
            return {Token::INTEGER, content};
        }

        auto wordToken = matchWordToken();
        if (wordToken.has_value()) {
            currentWord = currentWord.substr(wordToken.value().content.length(), currentWord.length() - 1);
            return wordToken.value();
        }

        auto twoCharToken = matchTwoCharToken();
        if (twoCharToken.has_value()) {
            currentWord = currentWord.substr(2, currentWord.length() - 1);
            return twoCharToken.value();
        }

        auto oneCharToken = matchOneCharacterToken();
        if (oneCharToken.has_value()) {
            currentWord = currentWord.substr(1, currentWord.length() - 1);
            return oneCharToken.value();
        }

        if (currentWord == previousWord) {
            break;
        } else {
            previousWord = currentWord;
        }
    }

    std::string invalidToken;
    auto nextSpace = currentWord.find(' ');
    if (nextSpace == -1) {
        invalidToken = currentWord;
        currentWord = "";
    } else {
        invalidToken = currentWord.substr(0, nextSpace);
        currentWord = currentWord.substr(nextSpace + 1, currentWord.length() - 1);
    }

    if (!invalidToken.empty() && verbose) {
        std::cout << "Found an invalid token: '" << invalidToken << "'" << std::endl;
    }
    return {Token::END_OF_FILE, invalidToken};
}

std::optional<Token> Lexer::matchWordToken() {
    if (currentWord.find("true") == 0) {
        return std::optional<Token>({Token::TRUE, "true"});
    } else if (currentWord.find("false") == 0) {
        return std::optional<Token>({Token::FALSE, "false"});
    } else if (currentWord.find("not") == 0) {
        return std::optional<Token>({Token::NOT, "not"});
    } else if (currentWord.find("and") == 0) {
        return std::optional<Token>({Token::AND, "and"});
    } else if (currentWord.find("or") == 0) {
        return std::optional<Token>({Token::OR, "or"});
    }
    return {};
}

std::optional<Token> Lexer::matchTwoCharToken() {
    if (currentWord.find("!=") == 0) {
        return std::optional<Token>({Token::NOT_EQUALS, "!="});
    } else if (currentWord.find("==") == 0) {
        return std::optional<Token>({Token::EQUALS, "=="});
    } else if (currentWord.find(">=") == 0) {
        return std::optional<Token>({Token::GREATER_EQUALS, ">="});
    } else if (currentWord.find("<=") == 0) {
        return std::optional<Token>({Token::LESS_EQUALS, "<="});
    }
    return {};
}

std::optional<Token> Lexer::matchOneCharacterToken() {
    char firstChar = currentWord[0];
    if (firstChar == '(') {
        return std::optional<Token>({Token::LEFT_PARAN, "("});
    } else if (firstChar == ')') {
        return std::optional<Token>({Token::RIGHT_PARAN, ")"});
    } else if (firstChar == '+') {
        return std::optional<Token>({Token::PLUS, "+"});
    } else if (firstChar == '-') {
        return std::optional<Token>({Token::MINUS, "-"});
    } else if (firstChar == '*') {
        return std::optional<Token>({Token::STAR, "*"});
    } else if (firstChar == '/') {
        return std::optional<Token>({Token::DIV, "/"});
    } else if (firstChar == ';') {
        return std::optional<Token>({Token::SEMICOLON, ";"});
    } else if (firstChar == '<') {
        return std::optional<Token>({Token::LESS_THAN, "<"});
    } else if (firstChar == '>') {
        return std::optional<Token>({Token::GREATER_THAN, ">"});
    }
    return {};
}
