#pragma once

#include <string>

namespace ast {

enum DataType { VOID, BOOL, INT, FLOAT, STRING };

enum NodeType {
    SEQUENCE,
    STATEMENT,
    LITERAL,
    UNARY_OPERATION,
    BINARY_OPERATION,
    FUNCTION,
    CALL,
    VARIABLE_DEFINITION,
    VARIABLE,
    ARRAY_ACCESS,
    ASSIGNMENT,
    IF_STATEMENT,
    FOR_STATEMENT,
    IMPORT,
};

enum BinaryOperationType {
    ADDITION,
    MULTIPLICATION,
    SUBTRACTION,
    DIVISION,
    EQUALS,
    NOT_EQUALS,
    LESS_EQUALS,
    LESS_THAN,
    GREATER_EQUALS,
    GREATER_THAN,
};

} // namespace ast

std::string to_string(ast::DataType dataType);
