#pragma once

#include <string>
#include <utility>

namespace ast {
enum SimpleDataType { VOID, BOOL, INT, FLOAT, STRING };
}

std::string to_string(ast::SimpleDataType type);

namespace ast {

struct DataType {
    std::string typeName = "";
    explicit DataType() : typeName(to_string(SimpleDataType::VOID)) {}
    explicit DataType(SimpleDataType simple) : typeName(to_string(simple)) {}
    explicit DataType(std::string typeName) : typeName(std::move(typeName)) {}
};

inline bool operator==(const DataType &lhs, const DataType &rhs) { return lhs.typeName == rhs.typeName; }
inline bool operator!=(const DataType &lhs, const DataType &rhs) { return lhs.typeName != rhs.typeName; }

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
    TYPE_DECLARATION,
    TYPE_MEMBER,
    MEMBER_ACCESS,
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

bool isSimpleDataType(const ast::DataType &type);

SimpleDataType toSimpleDataType(const ast::DataType &type);

} // namespace ast

std::string to_string(const ast::DataType &dataType);

std::string to_string(ast::BinaryOperationType operationType);

namespace std {

template <> struct hash<ast::DataType> {
    std::size_t operator()(const ast::DataType &k) const { return hash<string>()(k.typeName); }
};

} // namespace std
