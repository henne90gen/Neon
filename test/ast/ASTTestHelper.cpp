#include "ASTTestHelper.h"

#include <catch2/catch.hpp>
#include <iostream>

void assertAstsAreEqual(SimpleTree *expected, SimpleTree *actual) {
    INFO(std::to_string(expected->type) + " | " + std::to_string(actual->type));
    REQUIRE(expected->type == actual->type);
    REQUIRE(expected->children.size() == actual->children.size());
    for (unsigned long i = 0; i < expected->children.size(); i++) {
        assertAstsAreEqual(expected->children[i], actual->children[i]);
    }
}

SimpleTree *createSimpleFromSpecification(const std::vector<AstNodeSpec> &spec, int &index, int indentation = 0) {
    auto firstLine = spec[index];
    auto node = new SimpleTree();
    node->type = firstLine.type;
    while (index + 1 < spec.size() && spec[index + 1].indentation > indentation) {
        index++;
        auto child = createSimpleFromSpecification(spec, index, indentation + 1);
        node->children.push_back(child);
    }
    return node;
}

SimpleTree *createSimpleFromSequence(SequenceNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    for (auto child : node->getChildren()) {
        result->children.push_back(createSimpleFromAst(child));
    }
    return result;
}

SimpleTree *createSimpleFromStatement(StatementNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    result->children.push_back(createSimpleFromAst(node->getChild()));
    return result;
}

SimpleTree *createSimpleFromLiteral(LiteralNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    return result;
}

SimpleTree *createSimpleFromUnary(UnaryOperationNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    result->children.push_back(createSimpleFromAst(node->getChild()));
    return result;
}

SimpleTree *createSimpleFromBinary(BinaryOperationNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    result->children.push_back(createSimpleFromAst(node->getLeft()));
    result->children.push_back(createSimpleFromAst(node->getRight()));
    return result;
}

SimpleTree *createSimpleFromFunction(FunctionNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    for (auto argument : node->getArguments()) {
        result->children.push_back(createSimpleFromAst(argument));
    }
    result->children.push_back(createSimpleFromAst(node->getBody()));
    return result;
}

SimpleTree *createSimpleFromVariableDefinition(VariableDefinitionNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    return result;
}

SimpleTree *createSimpleFromVariable(VariableNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    return result;
}

SimpleTree *createSimpleFromAssignment(AssignmentNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    result->children.push_back(createSimpleFromAst(node->getLeft()));
    result->children.push_back(createSimpleFromAst(node->getRight()));
    return result;
}

SimpleTree *createSimpleFromCall(CallNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    return result;
}

SimpleTree *createSimpleFromAst(AstNode *node) {
    switch (node->getAstNodeType()) {
    case AstNode::SEQUENCE:
        return createSimpleFromSequence((SequenceNode *)node);
    case AstNode::STATEMENT:
        return createSimpleFromStatement((StatementNode *)node);
    case AstNode::LITERAL:
        return createSimpleFromLiteral((LiteralNode *)node);
    case AstNode::UNARY_OPERATION:
        return createSimpleFromUnary((UnaryOperationNode *)node);
    case AstNode::BINARY_OPERATION:
        return createSimpleFromBinary((BinaryOperationNode *)node);
    case AstNode::FUNCTION:
        return createSimpleFromFunction((FunctionNode *)node);
    case AstNode::VARIABLE_DEFINITION:
        return createSimpleFromVariableDefinition((VariableDefinitionNode *)node);
    case AstNode::VARIABLE:
        return createSimpleFromVariable((VariableNode *)node);
    case AstNode::ASSIGNMENT:
        return createSimpleFromAssignment((AssignmentNode *)node);
    case AstNode::CALL:
        return createSimpleFromCall((CallNode *)node);
    default:
        std::cerr << "Could not create simple helper tree node for " << to_string(node->getAstNodeType()) << std::endl;
        return nullptr;
    }
}

void assertProgramCreatesAst(const std::vector<std::string> &program, std::vector<AstNodeSpec> &spec) {
    int index = 0;
    auto expected = createSimpleFromSpecification(spec, index);
    CodeProvider *cp = new StringCodeProvider(program);
    Program prog = {};
    Lexer lexer(cp, prog, false);
    Parser parser(lexer, prog, false);
    ParseTreeNode *parseTree = parser.createParseTree();
    auto actual = createAstFromParseTree(parseTree);
    assertAstsAreEqual(expected, createSimpleFromAst(actual));
}