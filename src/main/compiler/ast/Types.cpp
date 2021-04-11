#include "Types.h"

#include <iostream>

std::string to_string(const ast::DataType &dataType) { return dataType.typeName; }

std::string to_string(ast::SimpleDataType type) {
    switch (type) {
    case ast::VOID:
        return "VOID";
    case ast::BOOLEAN:
        return "BOOL";
    case ast::INTEGER:
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
    case ast::BinaryOperationType::ADDITION:
        return "ADDITION";
    case ast::BinaryOperationType::MULTIPLICATION:
        return "MULTIPLICATION";
    case ast::BinaryOperationType::SUBTRACTION:
        return "SUBTRACTION";
    case ast::BinaryOperationType::DIVISION:
        return "DIVISION";
    case ast::BinaryOperationType::EQUALS:
        return "EQUALS";
    case ast::BinaryOperationType::NOT_EQUALS:
        return "NOT_EQUALS";
    case ast::BinaryOperationType::LESS_EQUALS:
        return "LESS_EQUALS";
    case ast::BinaryOperationType::LESS_THAN:
        return "LESS_THAN";
    case ast::BinaryOperationType::GREATER_EQUALS:
        return "GREATER_EQUALS";
    case ast::BinaryOperationType::GREATER_THAN:
        return "GREATER_THAN";
    case ast::BinaryOperationType::AND:
        return "AND";
    case ast::BinaryOperationType::OR:
        return "OR";
    }
    std::cerr << "Could not convert " << static_cast<int>(operationType) << " to string." << std::endl;
    exit(1);
}

std::string to_string(ast::UnaryOperationType operationType) {
    switch (operationType) {
    case ast::UnaryOperationType::NOT:
        return "NOT";
    case ast::UnaryOperationType::NEGATE:
        return "NEGATE";
    }
    std::cerr << "Could not convert " << static_cast<int>(operationType) << " to string." << std::endl;
    exit(1);
}

ast::SimpleDataType from_string(const std::string &type) {
    if (type == "void") {
        return ast::SimpleDataType::VOID;
    }
    if (type == "int") {
        return ast::SimpleDataType::INTEGER;
    }
    if (type == "float") {
        return ast::SimpleDataType::FLOAT;
    }
    if (type == "bool") {
        return ast::SimpleDataType::BOOLEAN;
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
    }
    if (type.typeName == "BOOL") {
        return ast::SimpleDataType::BOOLEAN;
    }
    if (type.typeName == "INT") {
        return ast::SimpleDataType::INTEGER;
    }
    if (type.typeName == "FLOAT") {
        return ast::SimpleDataType::FLOAT;
    }
    if (type.typeName == "STRING") {
        return ast::SimpleDataType::STRING;
    }
    return ast::SimpleDataType::VOID;
}
