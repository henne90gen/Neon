#pragma once

#include <unordered_map>

#include "AbstractSyntaxTree.h"

struct CalculationResult {
    enum CalculationType { INTEGER, FLOAT, BOOL };
    CalculationType type = INTEGER;
    union {
        int intResult;
        float floatResult;
        bool boolResult;
    };
};

class Interpreter {
  public:
    Interpreter(bool verbose = false) : verbose(verbose) {}

    void interpret(AstNode *node);

  private:
    bool verbose;
    std::unordered_map<AstNode *, CalculationResult> calculationResults;

    void interpretSEQ(SequenceNode *node);

    void interpretSTMT(StatementNode *node);

    void interpretBIN_OP(BinaryOperationNode *node);

    void interpretUN_OP(UnaryOperationNode *node);

    void interpretLIT(LiteralNode *node);
};
