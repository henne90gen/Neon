#pragma once

#include <vector>
#include <string>

enum GrammarSymbol {
    NOT_EQUALS = 0,
    LEFT_PARAN = 1,
    RIGHT_PARAN = 2,
    STAR = 3,
    PLUS = 4,
    COMMA = 5,
    MINUS = 6,
    DIV = 7,
    SEMICOLON = 8,
    LESS_THAN = 9,
    LESS_EQUALS = 10,
    SINGLE_EQUALS = 11,
    DOUBLE_EQUALS = 12,
    GREATER_THAN = 13,
    GREATER_EQUALS = 14,
    LEFT_BRACKET = 15,
    RIGHT_BRACKET = 16,
    AND = 17,
    ARRAY_ACCESS = 18,
    ARRAY_DEFINITION = 19,
    ASSIGNMENT = 20,
    BOOLEAN = 21,
    CALL = 22,
    CALL_ARGS = 23,
    CALL_HEADER = 24,
    CONJUNCTION = 25,
    DATA_TYPE = 26,
    DEFINITION = 27,
    DISJUNCTION = 28,
    ELSE = 29,
    ENDOFFILE = 30,
    EXPRESSION = 31,
    EXTERN = 32,
    EXTERNAL_FUNCTION = 33,
    EXTERNAL_FUNCTION_HEADER = 34,
    EXTERNAL_FUNCTION_RETURN = 35,
    FACTOR = 36,
    FLOAT = 37,
    FOR = 38,
    FOR_STATEMENT = 39,
    FOR_STATEMENT_BODY = 40,
    FUN = 41,
    FUNCTION = 42,
    FUNCTION_ARGS = 43,
    FUNCTION_BODY = 44,
    FUNCTION_HEADER = 45,
    FUNCTION_RETURN = 46,
    IDENTIFIER = 47,
    IF = 48,
    IF_STATEMENT = 49,
    IF_STATEMENT_BODY = 50,
    IF_STATEMENT_ELSE = 51,
    IF_STATEMENT_ELSE_BODY = 52,
    IMPORT = 53,
    IMPORT_STATEMENT = 54,
    INTEGER = 55,
    NEGATION = 56,
    NEW_LINE = 57,
    NOT = 58,
    OR = 59,
    PROGRAM = 60,
    RELATION = 61,
    RETURN = 62,
    STATEMENT = 63,
    STATEMENTS = 64,
    STRING = 65,
    SUM = 66,
    TERM = 67,
    TYPE = 68,
    TYPE_BODY = 69,
    TYPE_DECLARATION = 70,
    TYPE_MEMBER_DECLARATION = 71,
    TYPE_MEMBER_DECLARATIONS = 72,
    VARIABLE_DEFINITION = 73,
    LEFT_CURLY_BRACE = 74,
    RIGHT_CURLY_BRACE = 75
};

std::string to_string(GrammarSymbol symbol);

enum StateTransitionType {
    SHIFT,
    GOTO,
    REDUCE,
    ACCEPT,
    ERROR,
};

std::string to_string(StateTransitionType transition);

#define NUM_RULES 8

struct StateTransition {
    StateTransitionType type = StateTransitionType::ERROR;
    int nextStateIndex = 0;
    GrammarSymbol symbol = ENDOFFILE;
    unsigned int numRules = 0;
    GrammarSymbol rule[NUM_RULES] = {};
    StateTransition *next = nullptr;
};

std::string to_string(const StateTransition &action);

void initializeStateTransitionTable();

extern const StateTransition *stateTransitionTable[168][76];
