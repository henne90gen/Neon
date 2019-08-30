#include "Interpreter.h"

#include <functional>
#include <iostream>

void Interpreter::interpretSEQ(AstNode *node) {
  for (auto child : node->children) {
    interpret(child);
  }
}

void Interpreter::interpretSTMT(AstNode *node) {
  auto child = node->children[0];
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
  }
}

CalculationResult getTypedResult(CalculationResult &left,
                                 CalculationResult &right) {
  CalculationResult result = {CalculationResult::INTEGER};
  if (left.type == CalculationResult::FLOAT ||
      right.type == CalculationResult::FLOAT) {
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
  } else {
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
  } else {
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
  } else {
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
  } else {
    result.intResult = left.intResult / right.intResult;
  }
  return result;
}

void Interpreter::interpretBIN_OP(AstNode *node) {
  for (auto child : node->children) {
    if (calculationResults.find(child) != calculationResults.end()) {
      continue;
    }
    interpret(child);
  }

  auto leftResult = calculationResults[node->children[0]];
  auto rightResult = calculationResults[node->children[1]];
  CalculationResult result = {};
  switch (node->type) {
  case AstNode::BIN_OP_ADD:
    result = add(leftResult, rightResult);
    break;
  case AstNode::BIN_OP_SUB:
    result = subtract(leftResult, rightResult);
    break;
  case AstNode::BIN_OP_MUL:
    result = multiply(leftResult, rightResult);
    break;
  case AstNode::BIN_OP_DIV:
    result = divide(leftResult, rightResult);
    break;
  }
  calculationResults[node] = result;
}

void Interpreter::interpretUN_OP(AstNode *node) {}

void Interpreter::interpretLIT(AstNode *node) {
  CalculationResult result = {};
  if (node->type == AstNode::INT_LIT) {
    result = {CalculationResult::INTEGER};
    result.intResult = ((IntegerData *)node->data)->value;
  } else if (node->type == AstNode::FLOAT_LIT) {
    result = {CalculationResult::FLOAT};
    result.floatResult = ((FloatData *)node->data)->value;
  } else {
    std::cout << "Data type not supported yet!" << std::endl;
    return;
  }
  calculationResults[node] = result;
}

void Interpreter::interpret(AstNode *node) {
  if (verbose) {
    std::cout << "Interpreting " << to_string(node->type) << std::endl;
  }

  switch (node->type) {
  case AstNode::SEQUENCE:
    interpretSEQ(node);
    break;
  case AstNode::STATEMENT:
    interpretSTMT(node);
    break;
  case AstNode::BIN_OP_ADD:
  case AstNode::BIN_OP_SUB:
  case AstNode::BIN_OP_MUL:
  case AstNode::BIN_OP_DIV:
    interpretBIN_OP(node);
    break;
  case AstNode::UNARY_OP_NOT:
    interpretUN_OP(node);
    break;
  case AstNode::INT_LIT:
  case AstNode::FLOAT_LIT:
    interpretLIT(node);
    break;
  default:
    std::cout << "AST node " << to_string(node->type) << " not supported."
              << std::endl;
    break;
  }
}
