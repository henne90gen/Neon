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
    ASSIGNMENT_LEFT = 21,
    BOOLEAN = 22,
    CALL = 23,
    CALL_ARGS = 24,
    CALL_HEADER = 25,
    CONJUNCTION = 26,
    DATA_TYPE = 27,
    DEFINITION = 28,
    DISJUNCTION = 29,
    ELSE = 30,
    ENDOFFILE = 31,
    EXPRESSION = 32,
    EXTERN = 33,
    EXTERNAL_FUNCTION = 34,
    EXTERNAL_FUNCTION_HEADER = 35,
    EXTERNAL_FUNCTION_RETURN = 36,
    FACTOR = 37,
    FLOAT = 38,
    FOR = 39,
    FOR_STATEMENT = 40,
    FOR_STATEMENT_BODY = 41,
    FUN = 42,
    FUNCTION = 43,
    FUNCTION_ARGS = 44,
    FUNCTION_BODY = 45,
    FUNCTION_HEADER = 46,
    FUNCTION_RETURN = 47,
    IDENTIFIER = 48,
    IF = 49,
    IF_STATEMENT = 50,
    IF_STATEMENT_BODY = 51,
    IF_STATEMENT_ELSE = 52,
    IF_STATEMENT_ELSE_BODY = 53,
    IMPORT = 54,
    IMPORT_STATEMENT = 55,
    INTEGER = 56,
    MEMBER_ACCESS = 57,
    NEGATION = 58,
    NEW_LINE = 59,
    NOT = 60,
    OR = 61,
    PROGRAM = 62,
    RELATION = 63,
    RETURN = 64,
    STATEMENT = 65,
    STATEMENTS = 66,
    STRING = 67,
    SUM = 68,
    TERM = 69,
    TYPE = 70,
    TYPE_BODY = 71,
    TYPE_DECLARATION = 72,
    TYPE_MEMBER_DECLARATION = 73,
    TYPE_MEMBER_DECLARATIONS = 74,
    VARIABLE_DEFINITION = 75,
    LEFT_CURLY_BRACE = 76,
    RIGHT_CURLY_BRACE = 77
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

extern const StateTransition *stateTransitionTable[165][78];
