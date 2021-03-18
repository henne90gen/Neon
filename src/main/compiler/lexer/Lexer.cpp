#include "Lexer.h"

#include <fstream>
#include <iostream>
#include <optional>
#include <regex>

#include "../../Utils.h"

std::optional<std::string> ByteCodeProvider::getMoreCode() {
    if (size == 0) {
        return {};
    }

    std::string s;
    while (*data != '\n' && size > 0) {
        s += *data;
        data++;
        size--;
    }
    if (*data == '\n' && size > 0) {
        data++;
        size--;
    }
    return std::optional<std::string>(s);
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
            lines.push_back(line + "\n");
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
    if (addLineBreaks) {
        result += "\n";
    }
    lines.erase(lines.begin());
    return std::optional(result);
}

std::string removeLeadingWhitespace(const std::string &str) {
    std::string result = str;
    while (!result.empty() && (result[0] == ' ' || result[0] == '\t')) {
        result = result.substr(1, result.length());
    }
    return result;
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

        currentWord = removeLeadingWhitespace(currentWord);
        log.debug("Current word: '" + currentWord + "'");

        auto floatToken = matchRegex("^[0-9]+\\.[0-9]+", Token::FLOAT);
        if (floatToken.has_value()) {
            currentWord = currentWord.substr(floatToken.value().content.length(), currentWord.length() - 1);
            return floatToken.value();
        }

        auto intToken = matchRegex("^[0-9]+", Token::INTEGER);
        if (intToken.has_value()) {
            currentWord = currentWord.substr(intToken.value().content.length(), currentWord.length() - 1);
            return intToken.value();
        }

        auto stringToken = matchRegex("^\".*\"", Token::STRING);
        if (stringToken.has_value()) {
            currentWord = currentWord.substr(stringToken.value().content.length(), currentWord.length() - 1);
            return stringToken.value();
        }

        auto commentToken = matchRegex("^#.*(\\n|$)", Token::COMMENT);
        if (commentToken.has_value()) {
            currentWord = currentWord.substr(commentToken.value().content.length(), currentWord.length() - 1);
            return commentToken.value();
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

        auto oneCharToken = matchOneCharToken();
        if (oneCharToken.has_value()) {
            currentWord = currentWord.substr(1, currentWord.length() - 1);
            return oneCharToken.value();
        }

        auto memberAccessToken =
              matchRegex("^[a-zA-Z_][_a-zA-Z0-9]*(\\.[a-zA-Z_][_a-zA-Z0-9]*)+", Token::MEMBER_ACCESS);
        if (memberAccessToken.has_value()) {
            currentWord = currentWord.substr(memberAccessToken.value().content.length(), currentWord.length() - 1);
            return memberAccessToken.value();
        }

        auto variableNameToken = matchRegex("^[a-zA-Z_][_a-zA-Z0-9]*", Token::IDENTIFIER);
        if (variableNameToken.has_value()) {
            currentWord = currentWord.substr(variableNameToken.value().content.length(), currentWord.length() - 1);
            return variableNameToken.value();
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

    if (!invalidToken.empty()) {
        log.debug("Found an invalid token: '" + invalidToken + "'");
    }
    return {Token::INVALID, invalidToken};
}

std::optional<Token> Lexer::matchRegex(const std::string &regex, Token::TokenType tokenType) {
    std::regex re(regex);
    auto itr = std::sregex_iterator(currentWord.begin(), currentWord.end(), re);
    if (itr != std::sregex_iterator()) {
        auto content = static_cast<std::string>((*itr).str());
        Token token = {tokenType, content};
        return std::optional<Token>(token);
    }
    return {};
}

std::optional<Token> Lexer::matchWordToken() {
    if (STARTS_WITH(currentWord, "true")) {
        return TOKEN(Token::BOOLEAN, "true");
    }
    if (STARTS_WITH(currentWord, "false")) {
        return TOKEN(Token::BOOLEAN, "false");
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
    if (STARTS_WITH(currentWord, "type")) {
        return TOKEN(Token::TYPE, "type");
    }
    if (STARTS_WITH(currentWord, "int")) {
        return TOKEN(Token::SIMPLE_DATA_TYPE, "int");
    }
    if (STARTS_WITH(currentWord, "float")) {
        return TOKEN(Token::SIMPLE_DATA_TYPE, "float");
    }
    if (STARTS_WITH(currentWord, "bool")) {
        return TOKEN(Token::SIMPLE_DATA_TYPE, "bool");
    }
    if (STARTS_WITH(currentWord, "string")) {
        return TOKEN(Token::SIMPLE_DATA_TYPE, "string");
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
    if (STARTS_WITH(currentWord, "for")) {
        return TOKEN(Token::FOR, "for");
    }
    if (STARTS_WITH(currentWord, "import")) {
        return TOKEN(Token::IMPORT, "import");
    }
    if (STARTS_WITH(currentWord, "assert")) {
        return TOKEN(Token::ASSERT, "assert");
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

std::optional<Token> Lexer::matchOneCharToken() {
    char firstChar = currentWord[0];
    if (firstChar == '\n') {
        return TOKEN(Token::NEW_LINE, "\n");
    }
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
    if (firstChar == '[') {
        return TOKEN(Token::LEFT_BRACKET, "[");
    }
    if (firstChar == ']') {
        return TOKEN(Token::RIGHT_BRACKET, "]");
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
