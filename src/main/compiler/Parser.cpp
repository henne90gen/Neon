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

void printParseTreeTestCase(const ParseTreeNode *node, const Module *program) {
    if (node == nullptr) {
        return;
    }

    std::string programArrayStr = program->toArrayString();
    std::cout << std::endl;
    std::cout << "SECTION(\"can handle __\") {" << std::endl;

    std::cout << "    std::vector<std::string> program = {\"" << programArrayStr << "\"};" << std::endl;
    std::cout << "    assertProgramCreatesParseTree(program, parseTree);" << std::endl;
    std::cout << "}" << std::endl;
    std::cout << std::endl;
}

void printCurrentParseState(const StateTransition &action, const std::vector<int> &states,
                            const std::vector<ParseTreeNode *> &nodes) {
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

GrammarSymbol convertToGrammarSymbol(const Token &token) {
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
    case Token::COMMA:
        return GrammarSymbol::COMMA;
    case Token::SEMICOLON:
        return GrammarSymbol::SEMICOLON;
    case Token::TRUE:
        return GrammarSymbol::TRUE;
    case Token::FALSE:
        return GrammarSymbol::FALSE;
    case Token::NOT:
        return GrammarSymbol::NOT;
    case Token::SINGLE_EQUALS:
        return GrammarSymbol::SINGLE_EQUALS;
    case Token::DOUBLE_EQUALS:
        return GrammarSymbol::DOUBLE_EQUALS;
    case Token::NOT_EQUALS:
        return GrammarSymbol::NOT_EQUALS;
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
    case Token::FUN:
        return GrammarSymbol::FUN;
    case Token::VARIABLE_NAME:
        return GrammarSymbol::VARIABLE_NAME;
    case Token::SIMPLE_DATA_TYPE:
        return GrammarSymbol::DATA_TYPE;
    case Token::LEFT_CURLY_BRACE:
        return GrammarSymbol::LEFT_CURLY_BRACE;
    case Token::RIGHT_CURLY_BRACE:
        return GrammarSymbol::RIGHT_CURLY_BRACE;
    case Token::LEFT_BRACKET:
        return GrammarSymbol::LEFT_BRACKET;
    case Token::RIGHT_BRACKET:
        return GrammarSymbol::RIGHT_BRACKET;
    case Token::RETURN:
        return GrammarSymbol::RETURN;
    case Token::EXTERN:
        return GrammarSymbol::EXTERN;
    case Token::IF:
        return GrammarSymbol::IF;
    case Token::ELSE:
        return GrammarSymbol::ELSE;
    case Token::FOR:
        return GrammarSymbol::FOR;
    case Token::NEW_LINE:
        return GrammarSymbol::NEW_LINE;
    case Token::STRING:
        return GrammarSymbol::STRING;
    case Token::IMPORT:
        return GrammarSymbol::IMPORT;
    default:
        std::cout << "Could not convert token " << to_string(token.type) << " to grammar symbol." << std::endl;
        exit(1);
    }
    return GrammarSymbol::ENDOFFILE;
}

std::optional<StateTransition> Parser::getNextAction(int rowIndex, int columnIndex) {
    auto rootAction = stateTransitionTable[rowIndex][columnIndex];
    if (rootAction == nullptr) {
        return {};
    }

    if (rootAction->next == nullptr) {
        return std::optional(*rootAction);
    }

    auto currentAction = rootAction;
    while (currentAction != nullptr) {
        if (currentAction->type == StateTransitionType::ACCEPT) {
            if (verbose) {
                std::cout << "Found ACCEPT" << std::endl;
            }
            return std::optional(*currentAction);
        }
        currentAction = currentAction->next;
    }

    currentAction = rootAction;
    while (currentAction != nullptr) {
        if (currentAction->type == StateTransitionType::SHIFT) {
            if (verbose) {
                std::cout << "Preferring SHIFT over REDUCE" << std::endl;
            }
            return std::optional(*currentAction);
        }
        currentAction = currentAction->next;
    }

    if (verbose) {
        std::cout << "Choosing first action instead of best one" << std::endl;
    }
    return std::optional(*rootAction);
}

void Parser::executeShift(Token &token, std::vector<int> &states, StateTransition &action,
                          std::vector<ParseTreeNode *> &nodes) {
    token = lexer.getToken();

    states.push_back(action.nextStateIndex);
    auto newNode = new ParseTreeNode();
    newNode->symbol = convertToGrammarSymbol(token);
    newNode->token = token;
    nodes.push_back(newNode);
}

void Parser::executeGoto(std::vector<int> &states, StateTransition &action) { states.push_back(action.nextStateIndex); }

void Parser::executeReduce(std::vector<int> &states, StateTransition &action, std::vector<ParseTreeNode *> &nodes) {
    auto newNode = new ParseTreeNode();
    newNode->symbol = action.symbol;

    auto lastNode = nodes.back();
    nodes.pop_back();
    for (unsigned long i = 0; i < action.numRules; i++) {
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
    initializeStateTransitionTable();

    std::vector<ParseTreeNode *> nodes = {};
    std::vector<int> states = {};
    states.push_back(0);
    auto token = lexer.getToken();
    auto node = new ParseTreeNode();
    node->symbol = convertToGrammarSymbol(token);
    node->token = token;
    nodes.push_back(node);

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
            auto root = new ParseTreeNode();
            root->symbol = GrammarSymbol::PROGRAM;
            for (auto node : nodes) {
                root->children.push_back(node);
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
    if (verbose) {
        std::cout << std::endl << "Program:" << std::endl;
        for (auto &t : program->tokens) {
            std::cout << to_string(t.type) << ": " << t.content << std::endl;
        }
    }

    return nullptr;
}
