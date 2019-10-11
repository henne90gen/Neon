#pragma once

#include <string>
#include <utility>
#include <vector>

#include "ASTVisitor.h"

// TODO add file and function name
#define NOT_IMPLEMENTED                                                                                                \
    std::cout << __FILE__ << ":" << __FUNCTION__ << ": Not implemented yet." << std::endl;                             \
    exit(1);

class AstNode {
  public:
    enum AstNodeType {
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
    enum DataType { VOID, INT, FLOAT, BOOL };

    explicit AstNode(AstNodeType type) : type(type) {}

    virtual void accept(ASTVisitor *v) = 0;

    AstNodeType getAstNodeType() { return type; }

  private:
    AstNodeType type;
};

#include "nodes/AssignmentNode.h"
#include "nodes/BinaryOperationNode.h"
#include "nodes/FunctionNode.h"
#include "nodes/IfStatementNode.h"
#include "nodes/LiteralNodes.h"
#include "nodes/SequenceNode.h"
#include "nodes/StatementNode.h"
#include "nodes/UnaryOperationNode.h"
#include "nodes/VariableDefinitionNode.h"
#include "nodes/VariableNode.h"

std::string to_string(AstNode::AstNodeType type);
