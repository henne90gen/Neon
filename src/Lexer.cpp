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

Token Lexer::_getToken() {
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

        std::regex varNameRegex("^[a-zA-Z_][-_a-zA-Z0-9]*");
        itr = std::sregex_iterator(currentWord.begin(), currentWord.end(), varNameRegex);
        if (itr != std::sregex_iterator()) {
            auto content = static_cast<std::string>((*itr).str());
            currentWord = currentWord.substr(content.length(), currentWord.length() - 1);
            return {Token::VARIABLE_NAME, content};
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

Token Lexer::getToken() {
    auto token = _getToken();
    program.tokens.push_back(token);
    return token;
}

#define CONTAINS(word, search) word.find(search) == 0
#define TOKEN(type, content) std::optional<Token>({type, content});

std::optional<Token> Lexer::matchWordToken() {
    if (CONTAINS(currentWord, "true")) {
        return TOKEN(Token::TRUE, "true");
    } else if (CONTAINS(currentWord, "false")) {
        return TOKEN(Token::FALSE, "false");
    } else if (CONTAINS(currentWord, "not")) {
        return TOKEN(Token::NOT, "not");
    } else if (CONTAINS(currentWord, "and")) {
        return TOKEN(Token::AND, "and");
    } else if (CONTAINS(currentWord, "or")) {
        return TOKEN(Token::OR, "or");
    } else if (CONTAINS(currentWord, "fun")) {
        return TOKEN(Token::FUN, "fun");
    } else if (CONTAINS(currentWord, "int")) {
        return TOKEN(Token::DATA_TYPE, "int");
    } else if (CONTAINS(currentWord, "float")) {
        return TOKEN(Token::DATA_TYPE, "float");
    } else if (CONTAINS(currentWord, "bool")) {
        return TOKEN(Token::DATA_TYPE, "bool");
    } else if (CONTAINS(currentWord, "return")) {
        return TOKEN(Token::RETURN, "return");
    } else if (CONTAINS(currentWord, "extern")) {
        return TOKEN(Token::EXTERN, "extern");
    }

    return {};
}

std::optional<Token> Lexer::matchTwoCharToken() {
    if (CONTAINS(currentWord, "!=")) {
        return TOKEN(Token::NOT_EQUALS, "!=");
    } else if (CONTAINS(currentWord, "==")) {
        return TOKEN(Token::DOUBLE_EQUALS, "==");
    } else if (CONTAINS(currentWord, ">=")) {
        return TOKEN(Token::GREATER_EQUALS, ">=");
    } else if (CONTAINS(currentWord, "<=")) {
        return TOKEN(Token::LESS_EQUALS, "<=");
    }
    return {};
}

std::optional<Token> Lexer::matchOneCharacterToken() {
    char firstChar = currentWord[0];
    if (firstChar == '(') {
        return TOKEN(Token::LEFT_PARAN, "(");
    } else if (firstChar == ')') {
        return TOKEN(Token::RIGHT_PARAN, ")");
    } else if (firstChar == '{') {
        return TOKEN(Token::LEFT_CURLY_BRACE, "{");
    } else if (firstChar == '}') {
        return TOKEN(Token::RIGHT_CURLY_BRACE, "}");
    } else if (firstChar == '=') {
        return TOKEN(Token::SINGLE_EQUALS, "=");
    } else if (firstChar == '+') {
        return TOKEN(Token::PLUS, "+");
    } else if (firstChar == '-') {
        return TOKEN(Token::MINUS, "-");
    } else if (firstChar == '*') {
        return TOKEN(Token::STAR, "*");
    } else if (firstChar == '/') {
        return TOKEN(Token::DIV, "/");
    } else if (firstChar == ',') {
        return TOKEN(Token::COMMA, ",");
    } else if (firstChar == ';') {
        return TOKEN(Token::SEMICOLON, ";");
    } else if (firstChar == '<') {
        return TOKEN(Token::LESS_THAN, "<");
    } else if (firstChar == '>') {
        return TOKEN(Token::GREATER_THAN, ">");
    }
    return {};
}
