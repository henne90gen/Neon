#include "ASTInterpreter.h"

#include <iostream>

void ASTInterpreter::printStatementResult(AstNode *child) {
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

CalculationResult getTypedResult(CalculationResult &left, CalculationResult &right) {
    CalculationResult result = {CalculationResult::INTEGER, 0};
    if (left.type == CalculationResult::FLOAT || right.type == CalculationResult::FLOAT) {
        result.type = CalculationResult::FLOAT;
    }
    return result;
}

float castToFloat(CalculationResult &result) {
    if (result.type == CalculationResult::INTEGER) {
        return (float)result.intResult;
    } else {
        return result.floatResult;
    }
}

CalculationResult add(CalculationResult &left, CalculationResult &right) {
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

CalculationResult subtract(CalculationResult &left, CalculationResult &right) {
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

CalculationResult multiply(CalculationResult &left, CalculationResult &right) {
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

CalculationResult divide(CalculationResult &left, CalculationResult &right) {
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

CalculationResult negate(CalculationResult &calc) {
    CalculationResult result = {CalculationResult::BOOL, false};
    if (calc.type == CalculationResult::BOOL) {
        result.boolResult = !calc.boolResult;
    }
    return result;
}

void ASTInterpreter::visitFunctionNode(FunctionNode *node) {
    if (functions.find(node->getName()) != functions.end()) {
        std::cout << "Redefinition of function " << node->getName() << "!" << std::endl;
    } else {
        functions[node->getName()] = node;
    }
}

void ASTInterpreter::visitBinaryOperationNode(BinaryOperationNode *node) {
    if (calculationResults.find(node->getLeft()) == calculationResults.end()) {
        node->getLeft()->accept(this);
    }
    if (calculationResults.find(node->getRight()) == calculationResults.end()) {
        node->getRight()->accept(this);
    }

    auto leftResult = calculationResults[node->getLeft()];
    auto rightResult = calculationResults[node->getRight()];
    CalculationResult result = {};
    switch (node->getType()) {
    case BinaryOperationNode::ADDITION:
        result = add(leftResult, rightResult);
        break;
    case BinaryOperationNode::SUBTRACTION:
        result = subtract(leftResult, rightResult);
        break;
    case BinaryOperationNode::MULTIPLICATION:
        result = multiply(leftResult, rightResult);
        break;
    case BinaryOperationNode::DIVISION:
        result = divide(leftResult, rightResult);
        break;
    }
    calculationResults[node] = result;
}

void ASTInterpreter::visitUnaryOperationNode(UnaryOperationNode *node) {
    auto child = node->getChild();
    if (calculationResults.find(child) == calculationResults.end()) {
        child->accept(this);
    }

    auto childResult = calculationResults[child];

    CalculationResult result = {};
    switch (node->getType()) {
    case UnaryOperationNode::NOT:
        result = negate(childResult);
        break;
    }
    calculationResults[node] = result;
}

void ASTInterpreter::visitSequenceNode(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
}

void ASTInterpreter::visitStatementNode(StatementNode *node) {
    if (node->getChild() == nullptr) {
        std::cout << "Could not interpret child of StatementNode." << std::endl;
        return;
    }
    node->getChild()->accept(this);
    printStatementResult(node->getChild());
}

void ASTInterpreter::visitVariableNode(VariableNode *node) {
    if (variables.find(node->getName()) == variables.end()) {
        std::cout << "Use of undefined variable." << std::endl;
        return;
    }
    calculationResults[node] = calculationResults[variables[node->getName()]];
}

void ASTInterpreter::visitVariableDefinitionNode(VariableDefinitionNode *node) { variables[node->getName()] = node; }

void ASTInterpreter::visitFloatNode(FloatNode *node) {
    CalculationResult result = {CalculationResult::FLOAT, 0};
    result.floatResult = node->getValue();
    calculationResults[node] = result;
}

void ASTInterpreter::visitIntegerNode(IntegerNode *node) {
    CalculationResult result = {CalculationResult::INTEGER, 0};
    result.intResult = node->getValue();
    calculationResults[node] = result;
}

void ASTInterpreter::visitBoolNode(BoolNode *node) {
    CalculationResult result = {CalculationResult::BOOL, false};
    result.boolResult = node->getValue();
    calculationResults[node] = result;
}

void ASTInterpreter::visitAssignmentNode(AssignmentNode *node) {
    if (calculationResults.find(node->getRight()) == calculationResults.end()) {
        node->getRight()->accept(this);
    }
    calculationResults[node] = calculationResults[node->getRight()];
}

void ASTInterpreter::visitCallNode(CallNode *node) {
    // TODO implement
}

void interpretAst(AstNode *node, bool verbose) {
    if (node == nullptr) {
        std::cout << "Could not interpret node (nullptr)" << std::endl;
        return;
    }

    auto interpreter = new ASTInterpreter(verbose);
    node->accept(interpreter);
}
