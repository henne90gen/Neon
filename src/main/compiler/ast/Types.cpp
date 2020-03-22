#include "Types.h"

#include <iostream>

std::string to_string(ast::DataType dataType) {
    switch (dataType) {
    case ast::VOID:
        return "VOID";
    case ast::BOOL:
        return "BOOL";
    case ast::INT:
        return "INT";
    case ast::FLOAT:
        return "FLOAT";
    case ast::STRING:
        return "STRING";
    }
    std::cerr << "Could not convert " << dataType << " to string." << std::endl;
}

std::string to_string(ast::BinaryOperationType operationType) {
    switch (operationType) {
    case ast::ADDITION:
        return "ADDITION";
    case ast::MULTIPLICATION:
        return "MULTIPLICATION";
    case ast::SUBTRACTION:
        return "SUBTRACTION";
    case ast::DIVISION:
        return "DIVISION";
    case ast::EQUALS:
        return "EQUALS";
    case ast::NOT_EQUALS:
        return "NOT_EQUALS";
    case ast::LESS_EQUALS:
        return "LESS_EQUALS";
    case ast::LESS_THAN:
        return "LESS_THAN";
    case ast::GREATER_EQUALS:
        return "GREATER_EQUALS";
    case ast::GREATER_THAN:
        return "GREATER_THAN";
    }
    std::cerr << "Could not convert " << operationType << " to string." << std::endl;
}
