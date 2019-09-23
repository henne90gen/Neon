#include "Interpreter.h"

#include <functional>
#include <iostream>

void Interpreter::interpretSEQ(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        interpret(child);
    }
}

void Interpreter::interpretSTMT(StatementNode *node) {
    auto child = node->getChild();
    interpret(child);
    if (calculationResults.find(child) == calculationResults.end()) {
        std::cout << "STATEMENT did not produce a result." << std::endl;
        return;
    }

    auto result = calculationResults[child];
    calculationResults[node] = result;
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
    CalculationResult result = {CalculationResult::INTEGER};
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
    CalculationResult result = {CalculationResult::BOOL};
    if (calc.type == CalculationResult::BOOL) {
        result.boolResult = !calc.boolResult;
    }
    return result;
}

void Interpreter::interpretBIN_OP(BinaryOperationNode *node) {
    if (calculationResults.find(node->getLeft()) == calculationResults.end()) {
        interpret(node->getLeft());
    }
    if (calculationResults.find(node->getRight()) == calculationResults.end()) {
        interpret(node->getRight());
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

void Interpreter::interpretUN_OP(UnaryOperationNode *node) {
    auto child = node->getChild();
    if (calculationResults.find(child) == calculationResults.end()) {
        interpret(child);
    }

    auto childResult = calculationResults[child];

    CalculationResult result = {};
    switch (node->getType()) {
    case UnaryOperationNode::UNARY_OP_NOT:
        result = negate(childResult);
        break;
    }
    calculationResults[node] = result;
}

void Interpreter::interpretLIT(LiteralNode *node) {
    CalculationResult result = {};
    if (node->getLiteralType() == LiteralNode::INTEGER) {
        result = {CalculationResult::INTEGER};
        result.intResult = ((IntegerNode *)node)->getValue();
    } else if (node->getLiteralType() == LiteralNode::FLOAT) {
        result = {CalculationResult::FLOAT};
        result.floatResult = ((FloatNode *)node)->getValue();
    } else if (node->getLiteralType() == LiteralNode::BOOL) {
        result = {CalculationResult::BOOL};
        result.boolResult = ((BoolNode *)node)->getValue();
    } else {
        std::cout << "Data type not supported yet!" << std::endl;
        return;
    }
    calculationResults[node] = result;
}

void Interpreter::interpret(AstNode *node) {
    if (verbose) {
        std::cout << "Interpreting " << node->getAstNodeType() << std::endl;
    }

    if (node == nullptr) {
        std::cout << "Could not interpret node (nullptr)" << std::endl;
        return;
    }

    switch (node->getAstNodeType()) {
    case AstNode::SEQUENCE:
        interpretSEQ((SequenceNode *)node);
        break;
    case AstNode::STATEMENT:
        interpretSTMT((StatementNode *)node);
        break;
    case AstNode::BINARY_OPERATION:
        interpretBIN_OP((BinaryOperationNode *)node);
        break;
    case AstNode::UNARY_OPERATION:
        interpretUN_OP((UnaryOperationNode *)node);
        break;
    case AstNode::LITERAL:
        interpretLIT((LiteralNode *)node);
        break;
    default:
        std::cout << "AST node " << node->getAstNodeType() << " not supported." << std::endl;
        break;
    }
}
