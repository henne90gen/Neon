#pragma once

#include <string>
#include <vector>

#include "ast/Types.h"

struct FunctionArgument {
    std::string name;
    ast::DataType type;
};

struct FunctionSignature {
    std::string name;
    ast::DataType returnType;
    std::vector<FunctionArgument> arguments = {};
};

struct ComplexTypeMember {
    std::string name;
    ast::DataType type;
};

struct CompositeType {
    ast::DataType type;
    std::vector<ComplexTypeMember> members = {};
};
