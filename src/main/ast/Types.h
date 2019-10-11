#pragma once

namespace ast {

enum DataType { VOID, INT, FLOAT, BOOL };

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
    ASSIGNMENT,
    IF_STATEMENT,
};

}