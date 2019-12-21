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
    default:
        std::cerr << "Could not convert " << dataType << " to string." << std::endl;
    }
}
