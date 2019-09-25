
#include "Grammar.h"

std::string to_string(GrammarSymbol symbol) {
  switch (symbol) {
  case NOT_EQUALS:
    return "NOT_EQUALS";
  case LEFT_PARAN:
    return "LEFT_PARAN";
  case RIGHT_PARAN:
    return "RIGHT_PARAN";
  case STAR:
    return "STAR";
  case PLUS:
    return "PLUS";
  case COMMA:
    return "COMMA";
  case MINUS:
    return "MINUS";
  case DIV:
    return "DIV";
  case SEMICOLON:
    return "SEMICOLON";
  case LESS_THAN:
    return "LESS_THAN";
  case LESS_EQUALS:
    return "LESS_EQUALS";
  case EQUALS:
    return "EQUALS";
  case GREATER_THAN:
    return "GREATER_THAN";
  case GREATER_EQUALS:
    return "GREATER_EQUALS";
  case AND:
    return "AND";
  case CONJUNCTION:
    return "CONJUNCTION";
  case DATA_TYPE:
    return "DATA_TYPE";
  case DISJUNCTION:
    return "DISJUNCTION";
  case ENDOFFILE:
    return "ENDOFFILE";
  case EXPR:
    return "EXPR";
  case FACTOR:
    return "FACTOR";
  case FALSE:
    return "FALSE";
  case FLOAT:
    return "FLOAT";
  case FUN:
    return "FUN";
  case FUNCTION:
    return "FUNCTION";
  case FUNCTION_ARGS:
    return "FUNCTION_ARGS";
  case INTEGER:
    return "INTEGER";
  case NEGATION:
    return "NEGATION";
  case NOT:
    return "NOT";
  case OR:
    return "OR";
  case PROGRAM:
    return "PROGRAM";
  case RELATION:
    return "RELATION";
  case RETURN:
    return "RETURN";
  case STMT:
    return "STMT";
  case STMTS:
    return "STMTS";
  case SUM:
    return "SUM";
  case TERM:
    return "TERM";
  case TRUE:
    return "TRUE";
  case VARIABLE_DEFINITION:
    return "VARIABLE_DEFINITION";
  case VARIABLE_NAME:
    return "VARIABLE_NAME";
  case LEFT_CURLY_BRACE:
    return "LEFT_CURLY_BRACE";
  case RIGHT_CURLY_BRACE:
    return "RIGHT_CURLY_BRACE";
  }
}

std::string to_string(StateTransitionType transition) {
  switch (transition) {
    case SHIFT:
      return "SHIFT";
    case GOTO:
      return "GOTO";
    case REDUCE:
      return "REDUCE";
    case ACCEPT:
      return "ACCEPT";
    case ERROR:
      return "ERROR";
  }
}

std::string to_string(StateTransition &action) {
  std::string info = "UNKNOWN";
  if (action.type == StateTransitionType::ACCEPT) {
    info = "ACCEPT";
  } else if (action.type == StateTransitionType::ERROR) {
    info = "ERROR";
  } else if (action.type == StateTransitionType::GOTO) {
    info = "GOTO";
    info += ":" + std::to_string(action.nextStateIndex);
  } else if (action.type == StateTransitionType::SHIFT) {
    info = "SHIFT";
    info += ":" + std::to_string(action.nextStateIndex);
  } else if (action.type == StateTransitionType::REDUCE) {
    info = "REDUCE";
    info += ":" + to_string(action.symbol) + "->";
    for (auto rule : action.rule) {
      info += to_string(rule) + " ";
    }
    if (info[info.length() - 1] == ' ') {
      info = info.substr(0, info.length() - 1);
    }
  }
  std::string result = "Action (" + info + ")";
  return result;
}
