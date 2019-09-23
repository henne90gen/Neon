#include "Parser.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>

void printParseTree(ParseTreeNode *node, int indentation) {
    if (node == nullptr) {
        return;
    }
    for (int i = 0; i < indentation; i++) {
        std::cout << "  ";
    }
    std::cout << "Node " << to_string(node->symbol) << std::endl;
    for (auto child : node->children) {
        printParseTree(child, indentation + 1);
    }
}

void printParseTreeTestCase(ParseTreeNode *node, int indentation) {
    if (node == nullptr) {
        return;
    }
    std::string program;
    if (indentation == 0) {
        for (const auto &t : node->program) {
            program += t.content;
        }
        std::cout << std::endl;
        std::cout << "TEST_CASE(\"Parser can handle '" << program << "'\") {" << std::endl;
        std::cout << "    std::vector<std::pair<int, GrammarSymbol>> parseTree = {" << std::endl;
    }

    std::cout << "        {" << indentation << ",  GrammarSymbol::" << to_string(node->symbol) << "}," << std::endl;
    for (auto child : node->children) {
        printParseTreeTestCase(child, indentation + 1);
    }

    if (indentation == 0) {
        std::cout << "    };" << std::endl;
        std::cout << "    std::vector<std::string> program = {\"" << program << "\"};" << std::endl;
        std::cout << "    assertProgramCreatesParseTree(program, parseTree);" << std::endl;
        std::cout << "}" << std::endl;
        std::cout << std::endl;
    }
}

void printCurrentParseState(StateTransition &action, std::vector<int> &states, std::vector<ParseTreeNode *> &nodes) {
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
    case Token::INTEGER:
        return GrammarSymbol::INTEGER;
    case Token::FLOAT:
        return GrammarSymbol::FLOAT;
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
        return GrammarSymbol::ENDOFFILE;
    case Token::SEMICOLON:
        return GrammarSymbol::SEMICOLON;
    case Token::TRUE:
        return GrammarSymbol::TRUE;
    case Token::FALSE:
        return GrammarSymbol::FALSE;
    case Token::NOT:
        return GrammarSymbol::NOT;
    case Token::LESS_THAN:
        return GrammarSymbol::LESS_THAN;
    case Token::GREATER_THAN:
        return GrammarSymbol::GREATER_THAN;
    case Token::LESS_EQUALS:
        return GrammarSymbol::LESS_EQUALS;
    case Token::GREATER_EQUALS:
        return GrammarSymbol::GREATER_EQUALS;
    case Token::AND:
        return GrammarSymbol::AND;
    case Token::OR:
        return GrammarSymbol::OR;
    default:
        std::cout << "Could not convert token " << to_string(token.type) << " to grammar symbol." << std::endl;
        exit(1);
    }
    return GrammarSymbol::ENDOFFILE;
}

std::optional<StateTransition> Parser::getNextAction(int rowIndex, int columnIndex) {
    auto actions = stateTransitionTable[rowIndex][columnIndex];
    if (actions.empty()) {
        return {};
    }

    if (actions.size() == 1) {
        return std::optional(actions[0]);
    }

    for (auto &action : actions) {
        if (action.type == StateTransitionType::ACCEPT) {
            if (verbose) {
                std::cout << "Found ACCEPT" << std::endl;
            }
            return std::optional(action);
        }
    }

    for (auto &action : actions) {
        if (action.type == StateTransitionType::SHIFT) {
            if (verbose) {
                std::cout << "Preferring SHIFT over REDUCE" << std::endl;
            }
            return std::optional(action);
        }
    }

    if (verbose) {
        std::cout << "Choosing first action instead of best one" << std::endl;
    }
    return std::optional(actions[0]);
}

void Parser::executeShift(Token &token, std::vector<int> &states, StateTransition &action,
                          std::vector<ParseTreeNode *> &nodes) {
    token = lexer.getToken();

    // save token in program
    program.push_back(token);

    states.push_back(action.nextStateIndex);
    auto newNode = new ParseTreeNode(convertToGrammarSymbol(token), token);
    nodes.push_back(newNode);
}

void Parser::executeGoto(std::vector<int> &states, StateTransition &action) { states.push_back(action.nextStateIndex); }

void Parser::executeReduce(std::vector<int> &states, StateTransition &action, std::vector<ParseTreeNode *> &nodes) {
    auto newNode = new ParseTreeNode(action.symbol);
    auto lastNode = nodes.back();
    nodes.pop_back();
    for (auto &ruleElement : action.rule) {
        auto lastN = nodes.back();
        newNode->children.push_back(lastN);
        nodes.pop_back();
        states.pop_back();
    }
    std::reverse(newNode->children.begin(), newNode->children.end());
    nodes.push_back(newNode);
    nodes.push_back(lastNode);

    auto rowIndex = states.back();
    int columnIndex = action.symbol;
    auto newActionOptional = getNextAction(rowIndex, columnIndex);
    if (!newActionOptional) {
        std::cout << "This should never happen " << rowIndex << "," << columnIndex << std::endl;
        exit(1);
    }
    states.push_back(newActionOptional.value().nextStateIndex);
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
    program.push_back(token);
    nodes.push_back(new ParseTreeNode(convertToGrammarSymbol(token), token));

    while (true) {
        int rowIndex = states.back();
        int columnIndex = convertToGrammarSymbol(token);
        auto actionOptional = getNextAction(rowIndex, columnIndex);
        if (!actionOptional) {
            std::cout << "Could not get next action: " << rowIndex << ", " << columnIndex << std::endl;
            break;
        }
        auto action = actionOptional.value();

        if (verbose) {
            printCurrentParseState(action, states, nodes);
        }

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
            for (const auto &t : program) {
                root->program.push_back(t);
            }
            if (verbose) {
                std::cout << "Accepted program!" << std::endl;
            }
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
