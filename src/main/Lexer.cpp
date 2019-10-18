#include "Lexer.h"

#include <fstream>
#include <iostream>
#include <optional>
#include <regex>

#include "Utils.h"

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

        std::regex varNameRegex("^[a-zA-Z_][_a-zA-Z0-9]*");
        itr = std::sregex_iterator(currentWord.begin(), currentWord.end(), varNameRegex);
        if (itr != std::sregex_iterator()) {
            auto content = static_cast<std::string>((*itr).str());
            currentWord = currentWord.substr(content.length(), currentWord.length() - 1);
            return {Token::VARIABLE_NAME, content};
        }

        if (currentWord == previousWord) {
            break;
        }
        previousWord = currentWord;
    }

    std::string invalidToken;
    auto nextSpace = currentWord.find(' ');
    if (nextSpace == std::string::npos) {
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

std::optional<Token> Lexer::matchWordToken() {
    if (STARTS_WITH(currentWord, "true")) {
        return TOKEN(Token::TRUE, "true");
    }
    if (STARTS_WITH(currentWord, "false")) {
        return TOKEN(Token::FALSE, "false");
    }
    if (STARTS_WITH(currentWord, "not")) {
        return TOKEN(Token::NOT, "not");
    }
    if (STARTS_WITH(currentWord, "and")) {
        return TOKEN(Token::AND, "and");
    }
    if (STARTS_WITH(currentWord, "or")) {
        return TOKEN(Token::OR, "or");
    }
    if (STARTS_WITH(currentWord, "fun")) {
        return TOKEN(Token::FUN, "fun");
    }
    if (STARTS_WITH(currentWord, "int")) {
        return TOKEN(Token::DATA_TYPE, "int");
    }
    if (STARTS_WITH(currentWord, "float")) {
        return TOKEN(Token::DATA_TYPE, "float");
    }
    if (STARTS_WITH(currentWord, "bool")) {
        return TOKEN(Token::DATA_TYPE, "bool");
    }
    if (STARTS_WITH(currentWord, "return")) {
        return TOKEN(Token::RETURN, "return");
    }
    if (STARTS_WITH(currentWord, "extern")) {
        return TOKEN(Token::EXTERN, "extern");
    }
    if (STARTS_WITH(currentWord, "if")) {
        return TOKEN(Token::IF, "if");
    }
    if (STARTS_WITH(currentWord, "else")) {
        return TOKEN(Token::ELSE, "else");
    }

    return {};
}

std::optional<Token> Lexer::matchTwoCharToken() {
    if (STARTS_WITH(currentWord, "!=")) {
        return TOKEN(Token::NOT_EQUALS, "!=");
    }
    if (STARTS_WITH(currentWord, "==")) {
        return TOKEN(Token::DOUBLE_EQUALS, "==");
    }
    if (STARTS_WITH(currentWord, ">=")) {
        return TOKEN(Token::GREATER_EQUALS, ">=");
    }
    if (STARTS_WITH(currentWord, "<=")) {
        return TOKEN(Token::LESS_EQUALS, "<=");
    }
    return {};
}

std::optional<Token> Lexer::matchOneCharacterToken() {
    char firstChar = currentWord[0];
    if (firstChar == '(') {
        return TOKEN(Token::LEFT_PARAN, "(");
    }
    if (firstChar == ')') {
        return TOKEN(Token::RIGHT_PARAN, ")");
    }
    if (firstChar == '{') {
        return TOKEN(Token::LEFT_CURLY_BRACE, "{");
    }
    if (firstChar == '}') {
        return TOKEN(Token::RIGHT_CURLY_BRACE, "}");
    }
    if (firstChar == '=') {
        return TOKEN(Token::SINGLE_EQUALS, "=");
    }
    if (firstChar == '+') {
        return TOKEN(Token::PLUS, "+");
    }
    if (firstChar == '-') {
        return TOKEN(Token::MINUS, "-");
    }
    if (firstChar == '*') {
        return TOKEN(Token::STAR, "*");
    }
    if (firstChar == '/') {
        return TOKEN(Token::DIV, "/");
    }
    if (firstChar == ',') {
        return TOKEN(Token::COMMA, ",");
    }
    if (firstChar == ';') {
        return TOKEN(Token::SEMICOLON, ";");
    }
    if (firstChar == '<') {
        return TOKEN(Token::LESS_THAN, "<");
    }
    if (firstChar == '>') {
        return TOKEN(Token::GREATER_THAN, ">");
    }
    return {};
}
