#include "Parser.h"

#include <iostream>
#include <vector>

void printParseTree(ParseTreeNode *root, int indentation = 0) {
  for (int i = 0; i < indentation; i++) {
    std::cout << "  ";
  }
  std::cout << "Node " << to_string(root->symbol) << std::endl;
  for (auto node : root->children) {
    printParseTree(node, indentation + 1);
  }
}

void printCurrentParseState(StateTransition &action, std::vector<int> &states,
                            std::vector<ParseTreeNode *> &nodes) {
  for (auto node : nodes) {
      std::cout << to_string(node->symbol) + " ";
  }
  std::cout << std::endl;
  std::cout << to_string(action) << " | ";
  for (auto s : states) {
    std::cout << s << ", ";
  }
  std::cout << std::endl;
}

GrammarSymbol convertToGrammarSymbol(Token &token) {
  switch (token.type) {
  case Token::INT_LIT:
    return GrammarSymbol::INT_LIT;
  case Token::FLOAT_LIT:
    return GrammarSymbol::FLOAT_LIT;
  case Token::PLUS:
    return GrammarSymbol::PLUS;
  case Token::MINUS:
    return GrammarSymbol::MINUS;
  case Token::STAR:
    return GrammarSymbol::STAR;
  case Token::DIV:
    return GrammarSymbol::DIV;
  case Token::LEFT_PARAN:
    return GrammarSymbol::LEFT_PARAN;
  case Token::RIGHT_PARAN:
    return GrammarSymbol::RIGHT_PARAN;
  case Token::END_OF_FILE:
    return GrammarSymbol::END_OF_FILE;
  }
  return END_OF_FILE;
}

bool getNextAction(int rowIndex, int columnIndex, StateTransition &action) {
  auto actions = stateTransitionTable[rowIndex][columnIndex];
  if (actions.size() == 0) {
    return true;
  }

  if (actions.size() == 1) {
    action = actions[0];
    return false;
  }

  bool foundAccept = false;
  for (auto &act : actions) {
    if (act.type == StateTransitionType::ACCEPT) {
      action = act;
      foundAccept = true;
      std::cout << "Found ACCEPT" << std::endl;
      break;
    }
  }
  if (foundAccept) {
    return false;
  }

  bool foundShift = false;
  for (auto &act : actions) {
    if (act.type == StateTransitionType::SHIFT) {
      action = act;
      foundShift = true;
      std::cout << "Preferring SHIFT over REDUCE" << std::endl;
      break;
    }
  }
  if (foundShift) {
    return false;
  }

  action = actions[0];
  std::cout << "Choosing first action instead of best one" << std::endl;
  return false;
}

void Parser::executeShift(Token &token, std::vector<int> &states,
                          StateTransition &action,
                          std::vector<ParseTreeNode *> &nodes) {
  token = lexer.getToken();
  states.push_back(action.nextStateIndex);
  auto newNode = new ParseTreeNode(convertToGrammarSymbol(token));
  nodes.push_back(newNode);
}

void Parser::executeGoto(std::vector<int> &states, StateTransition &action) {
  states.push_back(action.nextStateIndex);
}

void Parser::executeReduce(std::vector<int> &states, StateTransition &action,
                           std::vector<ParseTreeNode *> &nodes) {
  auto newNode = new ParseTreeNode(action.symbol);
  for (auto &ruleElement : action.rule) {
    auto lastNode = nodes.back();
    newNode->children.push_back(lastNode);
    nodes.pop_back();
    states.pop_back();
  }
  nodes.push_back(newNode);

  auto rowIndex = states.back();
  int columnIndex = action.symbol;
  StateTransition newAction = {};
  bool err = getNextAction(rowIndex, columnIndex, newAction);
  if (err) {
    std::cout << "This should never happen " << rowIndex << "," << columnIndex
              << std::endl;
    exit(1);
  }
  states.push_back(newAction.nextStateIndex);
}

/**
 * Init: Push start symbol to the stack
 * Repeat until “accept” or “error”
 *   - Let S_top be the top of the stack and ‘a’ the next input
 *   - Compute action from table act = M[S_top, a]
 *   - If act = Shift S_y
 *       - Push S_y to stack and read next symbol
 *   - If act = Reduce(n)(with r_n: A -> a)
 *       - Remove |a| states from stack (i.e., roll back)
 *       - Do not advance input cursor
 *       - Compute new state M[S_top, A] = goto S_z, and push to stack
 *   - If act = error/accept: Report and stop
 */
ParseTreeNode *Parser::createParseTree() {
  std::vector<ParseTreeNode *> nodes = {};
  std::vector<int> states = {};
  states.push_back(0);
  auto token = lexer.getToken();
  nodes.push_back(new ParseTreeNode(convertToGrammarSymbol(token)));

  while (true) {
    int columnIndex = convertToGrammarSymbol(token);
    StateTransition action;
    bool err = getNextAction(states.back(), columnIndex, action);
    if (err) {
      std::cout << "Could not get next action" << std::endl;
      break;
    }

    printCurrentParseState(action, states, nodes);

    switch (action.type) {
    case SHIFT:
      executeShift(token, states, action, nodes);
      break;
    case GOTO:
      executeGoto(states, action);
      break;
    case REDUCE: {
      executeReduce(states, action, nodes);
    } break;
    case ACCEPT: {
      auto root = new ParseTreeNode(GrammarSymbol::PROGRAM);
      for (auto node : nodes) {
        root->children.push_back(node);
      }
      printParseTree(root);
      std::cout << "Accepted program!" << std::endl;
      return root;
    }
    case ERROR:
      std::cout << "ERROR!" << std::endl;
      return nullptr;
    }
  }

  std::cout << "Could not accept program!" << std::endl;
  return nullptr;
}
