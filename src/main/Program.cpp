#include "Program.h"

std::string trim(const std::string &s) { return rtrim(ltrim(s)); }

std::string rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

bool tokenHasSpaceBeforeIt(const Token *prev, const Token &t, const Token *next) {
    bool prevTokenIsEquals =
          prev != nullptr && (prev->type == Token::SINGLE_EQUALS || prev->type == Token::DOUBLE_EQUALS);
    bool isNumberAndAfterEquals = (t.type == Token::INTEGER || t.type == Token::FLOAT) && prevTokenIsEquals;
    return isNumberAndAfterEquals || t.type == Token::NEW_LINE || t.type == Token::VARIABLE_NAME ||
           t.type == Token::TRUE || t.type == Token::FALSE || t.type == Token::SIMPLE_DATA_TYPE ||
           t.type == Token::SINGLE_EQUALS || t.type == Token::DOUBLE_EQUALS;
}

std::string Program::toString() const {
    std::string programStr;
    bool isFirstToken = true;
    for (unsigned long i = 0; i < tokens.size(); i++) {
        const Token *prev = nullptr;
        if (i > 0) {
            prev = &tokens[i - 1];
        }
        const auto &t = tokens[i];
        const Token *next = nullptr;
        if (i + 1 < tokens.size()) {
            next = &tokens[i + 1];
        }
        if (!isFirstToken && tokenHasSpaceBeforeIt(prev, t, next)) {
            programStr += " ";
        }
        programStr += t.content;
        isFirstToken = false;
    }
    return trim(programStr);
}
