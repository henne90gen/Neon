#pragma once

#include <string>
#include <utility>

namespace ast {
enum SimpleDataType { VOID, BOOLEAN, INTEGER, FLOAT, STRING };
}

std::string to_string(ast::SimpleDataType type);
ast::SimpleDataType from_string(const std::string &type);

namespace ast {

struct DataType {
    std::string typeName;
    explicit DataType() : typeName(to_string(SimpleDataType::VOID)) {}
    explicit DataType(SimpleDataType simple) : typeName(to_string(simple)) {}
    explicit DataType(std::string typeName) : typeName(std::move(typeName)) {}
};

inline bool operator==(const DataType &lhs, const DataType &rhs) { return lhs.typeName == rhs.typeName; }
inline bool operator!=(const DataType &lhs, const DataType &rhs) { return lhs.typeName != rhs.typeName; }

enum class NodeType {
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
    FOR_STATEMENT,
    IMPORT,
    TYPE_DECLARATION,
    TYPE_MEMBER,
    MEMBER_ACCESS,
    ASSERT,
    COMMENT,
};

enum class BinaryOperationType {
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
    AND,
    OR,
};

enum class UnaryOperationType {
    NOT,
    NEGATE,
};

bool isSimpleDataType(const ast::DataType &type);

SimpleDataType toSimpleDataType(const ast::DataType &type);

} // namespace ast

std::string to_string(const ast::DataType &dataType);

std::string to_string(ast::BinaryOperationType operationType);
std::string to_string(ast::UnaryOperationType operationType);

namespace std {

template <> struct hash<ast::DataType> {
    std::size_t operator()(const ast::DataType &k) const { return hash<string>()(k.typeName); }
};

} // namespace std
