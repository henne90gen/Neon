#include "AstInterpreter.h"

#include "../nodes/AllNodes.h"
#include "util/Utils.h"

#include <iostream>

void AstInterpreter::printStatementResult(AstNode *child) {
    if (calculationResults.find(child) == calculationResults.end()) {
        std::cout << "Calculation did not produce a result." << std::endl;
        return;
    }

    auto result = calculationResults[child];
    if (result.type == CalculationResult::INTEGER) {
        std::cout << result.intResult << std::endl;
    } else if (result.type == CalculationResult::FLOAT) {
        std::cout << result.floatResult << std::endl;
    } else if (result.type == CalculationResult::BOOL) {
        std::string str = result.boolResult ? "true" : "false";
        std::cout << str << std::endl;
    } else {
        std::cout << "Could not print result." << std::endl;
    }
}

CalculationResult getTypedResult(const CalculationResult &left, const CalculationResult &right) {
    CalculationResult result = {CalculationResult::INTEGER, {0}};
    if (left.type == CalculationResult::FLOAT || right.type == CalculationResult::FLOAT) {
        result.type = CalculationResult::FLOAT;
    }
    return result;
}

float castToFloat(const CalculationResult &result) {
    if (result.type == CalculationResult::INTEGER) {
        return static_cast<float>(result.intResult);
    }
    return result.floatResult;
}

CalculationResult add(const CalculationResult &left, const CalculationResult &right) {
    CalculationResult result = getTypedResult(left, right);
    if (result.type == CalculationResult::FLOAT) {
        float l = castToFloat(left);
        float r = castToFloat(right);
        result.floatResult = l + r;
    } else if (result.type == CalculationResult::INTEGER) {
        result.intResult = left.intResult + right.intResult;
    }
    return result;
}

CalculationResult subtract(const CalculationResult &left, const CalculationResult &right) {
    CalculationResult result = getTypedResult(left, right);
    if (result.type == CalculationResult::FLOAT) {
        float l = castToFloat(left);
        float r = castToFloat(right);
        result.floatResult = l - r;
    } else if (result.type == CalculationResult::INTEGER) {
        result.intResult = left.intResult - right.intResult;
    }
    return result;
}

CalculationResult multiply(const CalculationResult &left, const CalculationResult &right) {
    CalculationResult result = getTypedResult(left, right);
    if (result.type == CalculationResult::FLOAT) {
        float l = castToFloat(left);
        float r = castToFloat(right);
        result.floatResult = l * r;
    } else if (result.type == CalculationResult::INTEGER) {
        result.intResult = left.intResult * right.intResult;
    }
    return result;
}

CalculationResult divide(const CalculationResult &left, const CalculationResult &right) {
    CalculationResult result = getTypedResult(left, right);
    if (result.type == CalculationResult::FLOAT) {
        float l = castToFloat(left);
        float r = castToFloat(right);
        result.floatResult = l / r;
    } else if (result.type == CalculationResult::INTEGER) {
        result.intResult = left.intResult / right.intResult;
    }
    return result;
}

CalculationResult negate(const CalculationResult &calc) {
    CalculationResult result = {CalculationResult::BOOL, {0}};
    if (calc.type == CalculationResult::BOOL) {
        result.boolResult = !calc.boolResult;
    }
    return result;
}

void AstInterpreter::visitFunctionNode(FunctionNode *node) {
    if (functions.find(node->getName()) != functions.end()) {
        std::cout << "Redefinition of function " << node->getName() << "!" << std::endl;
    } else {
        functions[node->getName()] = node;
    }
}

void AstInterpreter::visitBinaryOperationNode(BinaryOperationNode *node) {
    if (calculationResults.find(node->getLeft()) == calculationResults.end()) {
        node->getLeft()->accept(this);
    }
    if (calculationResults.find(node->getRight()) == calculationResults.end()) {
        node->getRight()->accept(this);
    }

    auto leftResult = calculationResults[node->getLeft()];
    auto rightResult = calculationResults[node->getRight()];
    CalculationResult result = {};
    NEON_NOT_IMPLEMENTED
    switch (node->getType()) {
    case ast::BinaryOperationType::ADDITION:
        result = add(leftResult, rightResult);
        break;
    case ast::BinaryOperationType::SUBTRACTION:
        result = subtract(leftResult, rightResult);
        break;
    case ast::BinaryOperationType::MULTIPLICATION:
        result = multiply(leftResult, rightResult);
        break;
    case ast::BinaryOperationType::DIVISION:
        result = divide(leftResult, rightResult);
        break;
    }
    calculationResults[node] = result;
}

void AstInterpreter::visitUnaryOperationNode(UnaryOperationNode *node) {
    auto child = node->getChild();
    if (calculationResults.find(child) == calculationResults.end()) {
        child->accept(this);
    }

    auto childResult = calculationResults[child];

    CalculationResult result = {};
    switch (node->getType()) {
    case ast::UnaryOperationType::NOT:
        result = negate(childResult);
        break;
    }
    calculationResults[node] = result;
}

void AstInterpreter::visitSequenceNode(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
}

void AstInterpreter::visitStatementNode(StatementNode *node) {
    if (node->getChild() == nullptr) {
        std::cout << "Could not interpret child of StatementNode." << std::endl;
        return;
    }
    node->getChild()->accept(this);
    printStatementResult(node->getChild());
}

void AstInterpreter::visitVariableNode(VariableNode *node) {
    if (variables.find(node->getName()) == variables.end()) {
        std::cout << "Use of undefined variable." << std::endl;
        return;
    }
    calculationResults[node] = calculationResults[variables[node->getName()]];
}

void AstInterpreter::visitVariableDefinitionNode(VariableDefinitionNode *node) { variables[node->getName()] = node; }

void AstInterpreter::visitFloatNode(FloatNode *node) {
    CalculationResult result = {CalculationResult::FLOAT, {0}};
    result.floatResult = node->getValue();
    calculationResults[node] = result;
}

void AstInterpreter::visitIntegerNode(IntegerNode *node) {
    CalculationResult result = {CalculationResult::INTEGER, {0}};
    result.intResult = node->getValue();
    calculationResults[node] = result;
}

void AstInterpreter::visitBoolNode(BoolNode *node) {
    CalculationResult result = {CalculationResult::BOOL, {0}};
    result.boolResult = node->getValue();
    calculationResults[node] = result;
}

void AstInterpreter::visitAssignmentNode(AssignmentNode *node) {
    if (calculationResults.find(node->getRight()) == calculationResults.end()) {
        node->getRight()->accept(this);
    }
    calculationResults[node] = calculationResults[node->getRight()];
}

void AstInterpreter::visitCallNode(CallNode * /*node*/) { NEON_NOT_IMPLEMENTED }
void AstInterpreter::visitIfStatementNode(IfStatementNode * /*node*/) { NEON_NOT_IMPLEMENTED }
void AstInterpreter::visitForStatementNode(ForStatementNode * /*node*/) { NEON_NOT_IMPLEMENTED }

void interpretAst(AstNode *node, bool verbose) {
    if (node == nullptr) {
        std::cout << "Could not interpret node (nullptr)" << std::endl;
        return;
    }

    auto interpreter = new AstInterpreter(verbose);
    node->accept(interpreter);
}
