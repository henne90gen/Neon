#pragma once

#include "../Parser.h"
#include "nodes/AllNodes.h"

class AstGenerator {
  public:
    explicit AstGenerator(Module *module) : module(module) {}

    void run(ParseTreeNode *root);

    std::vector<std::string> importedModules;
  private:
    Module *module;

    AstNode *createAstFromParseTree(ParseTreeNode *node);
    AstNode *createBinaryOperation(ParseTreeNode *node);
    AstNode *createUnaryOperation(ParseTreeNode *node);
    VariableNode *createVariable(ParseTreeNode *node);
    VariableDefinitionNode *createVariableDefinition(ParseTreeNode *node);
    AstNode *createSequence(ParseTreeNode *node, SequenceNode *seqRoot);
    StatementNode *createStatement(ParseTreeNode *node);
    void addArguments(FunctionNode *function, ParseTreeNode *root);
    void addArguments(CallNode *call, ParseTreeNode *root);
    FunctionNode *createExternalFunction(ParseTreeNode *node);
    FunctionNode *createFunction(ParseTreeNode *node);
    AssignmentNode *createAssignment(ParseTreeNode *node);
    CallNode *createCall(ParseTreeNode *node);
    ForStatementNode *createForStatement(ParseTreeNode *node);
    IfStatementNode *createIfStatement(ParseTreeNode *node);
    AstNode *createImportStatement(ParseTreeNode *node);
};
