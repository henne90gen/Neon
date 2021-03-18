#pragma once

#include "../../Module.h"
#include "../Logger.h"
#include "../ast/nodes/AllNodes.h"

class Parser {
  public:
    Parser(Lexer &lexer, Module *module, const Logger &logger) : lexer(lexer), module(module), log(logger) {}

    void run();

  private:
    Lexer &lexer;
    Module *module;
    const Logger &log;

    Token getNextToken();

    StatementNode *parseStatement(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    AssertNode *parseAssert(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    StatementNode *parseReturnStatement(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    ForStatementNode *parseFor(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    IfStatementNode *parseIf(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    AssignmentNode *parseAssignment(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    FunctionNode *parseFunction(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    SequenceNode *parseScope(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    VariableDefinitionNode *parseVariableDefinition(const std::vector<Token> &tokens, int &currentTokenIdx,
                                                    int level) const;
    CallNode *parseFunctionCall(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    VariableNode *parseVariable(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    AstNode *parseExpression(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    AstNode *parseAssignmentLeft(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    BinaryOperationNode *parseBinaryOperation(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    LiteralNode *parseLiteral(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    AstNode *parseBinaryLeft(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    AstNode *parseExpressionInsideParens(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    AstNode *parseBinaryRight(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
    CommentNode *parseComment(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const;
};
