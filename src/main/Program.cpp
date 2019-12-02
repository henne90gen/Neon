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
    bool prevExists = prev != nullptr;
    bool prevTokenIsEquals = prevExists && (prev->type == Token::SINGLE_EQUALS || prev->type == Token::DOUBLE_EQUALS);
    bool prevTokenIsOperation = prevExists && (prev->type == Token::PLUS || prev->type == Token::STAR ||
                                               prev->type == Token::MINUS || prev->type == Token::DIV);
    bool prevTokenIsParenthesis = prevExists && prev->type == Token::LEFT_PARAN;
    bool prevTokenIsUnaryOperator = prevExists && prev->type == Token::NOT;
    bool prevTokenIsBracket = prevExists && prev->type == Token::LEFT_BRACKET;

    bool isNumber = t.type == Token::INTEGER || t.type == Token::FLOAT;
    bool isNumberAndAfterEquals = isNumber && prevTokenIsEquals;
    bool isNumberAndAfterOperation = isNumber && prevTokenIsOperation;
    bool isNumberAndNotAfterBracket = isNumber && !prevTokenIsBracket && !prevTokenIsParenthesis;

    bool isVariable = t.type == Token::VARIABLE_NAME;
    bool isVariableAndNotAfterParenthesis = isVariable && !prevTokenIsParenthesis;

    bool isCurlyBrace = t.type == Token::LEFT_CURLY_BRACE || t.type == Token::RIGHT_CURLY_BRACE;

    bool isBinaryOperation = t.type == Token::PLUS || t.type == Token::STAR || t.type == Token::MINUS ||
                             t.type == Token::DIV || t.type == Token::LESS_THAN || t.type == Token::GREATER_THAN ||
                             t.type == Token::LESS_EQUALS || t.type == Token::GREATER_EQUALS || t.type == Token::AND ||
                             t.type == Token::OR;

    bool isParenthesisAndAfterUnaryOperator = t.type == Token::LEFT_PARAN && prevTokenIsUnaryOperator;

    bool isSimpleDataTypeAndNotAfterParenthesis = t.type == Token::SIMPLE_DATA_TYPE && !prevTokenIsParenthesis;

    return isNumberAndAfterEquals || isNumberAndAfterOperation || isNumberAndNotAfterBracket ||
           isVariableAndNotAfterParenthesis || isCurlyBrace || isBinaryOperation ||
           isParenthesisAndAfterUnaryOperator || isSimpleDataTypeAndNotAfterParenthesis || t.type == Token::NEW_LINE ||
           t.type == Token::TRUE || t.type == Token::FALSE || t.type == Token::SINGLE_EQUALS ||
           t.type == Token::DOUBLE_EQUALS || t.type == Token::ELSE || t.type == Token::NOT || t.type == Token::FUN;
}

std::string Program::toString() const {
    std::string programStr;
    bool isFirstToken = true;
    unsigned long tokenCount = tokens.size();
    for (unsigned long i = 0; i < tokenCount; i++) {
        const auto &t = tokens[i];
        if (i == tokenCount - 2 && t.type == Token::NEW_LINE) {
            // looking at second to last token, since last token is END_OF_FILE
            continue;
        }
        const Token *prev = nullptr;
        if (i > 0) {
            prev = &tokens[i - 1];
        }
        const Token *next = nullptr;
        if (i + 1 < tokenCount) {
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
