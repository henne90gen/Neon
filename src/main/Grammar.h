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
    IF = 47,
    IF_STATEMENT = 48,
    IF_STATEMENT_BODY = 49,
    IF_STATEMENT_ELSE = 50,
    IF_STATEMENT_ELSE_BODY = 51,
    IMPORT = 52,
    IMPORT_STATEMENT = 53,
    INTEGER = 54,
    NEGATION = 55,
    NEW_LINE = 56,
    NOT = 57,
    OR = 58,
    PROGRAM = 59,
    RELATION = 60,
    RETURN = 61,
    STATEMENT = 62,
    STATEMENTS = 63,
    STRING = 64,
    SUM = 65,
    TERM = 66,
    VARIABLE_DEFINITION = 67,
    VARIABLE_NAME = 68,
    LEFT_CURLY_BRACE = 69,
    RIGHT_CURLY_BRACE = 70
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

extern const StateTransition *stateTransitionTable[152][71];
