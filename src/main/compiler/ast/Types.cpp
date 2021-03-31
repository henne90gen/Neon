#include "Types.h"

#include <iostream>

std::string to_string(const ast::DataType &dataType) { return dataType.typeName; }

std::string to_string(ast::SimpleDataType type) {
    switch (type) {
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
    std::cerr << "Could not convert " << type << " to string." << std::endl;
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
    case ast::AND:
        return "AND";
    case ast::OR:
        return "OR";
    }
    std::cerr << "Could not convert " << operationType << " to string." << std::endl;
    exit(1);
}

std::string to_string(ast::UnaryOperationType operationType) {
    switch (operationType) {
    case ast::NOT:
        return "NOT";
    case ast::NEGATE:
        return "NEGATE";
    }
    std::cerr << "Could not convert " << operationType << " to string." << std::endl;
    exit(1);
}

ast::SimpleDataType from_string(const std::string &type) {
    if (type == "void") {
        return ast::SimpleDataType::VOID;
    }
    if (type == "int") {
        return ast::SimpleDataType::INT;
    }
    if (type == "float") {
        return ast::SimpleDataType::FLOAT;
    }
    if (type == "bool") {
        return ast::SimpleDataType::BOOL;
    }
    if (type == "string") {
        return ast::SimpleDataType::STRING;
    }
    std::cerr << "Could not convert " << type << " to a simple data type." << std::endl;
    exit(1);
}

bool ast::isSimpleDataType(const ast::DataType &type) {
    return type.typeName == "VOID" || type.typeName == "BOOL" || type.typeName == "INT" || type.typeName == "FLOAT" ||
           type.typeName == "STRING";
}

ast::SimpleDataType ast::toSimpleDataType(const ast::DataType &type) {
    if (type.typeName == "VOID") {
        return ast::SimpleDataType::VOID;
    } if (type.typeName == "BOOL") {
        return ast::SimpleDataType::BOOL;
    } if (type.typeName == "INT") {
        return ast::SimpleDataType::INT;
    } if (type.typeName == "FLOAT") {
        return ast::SimpleDataType::FLOAT;
    } else if (type.typeName == "STRING") {
        return ast::SimpleDataType::STRING;
    }
    return ast::SimpleDataType::VOID;
}
