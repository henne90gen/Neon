import copy
from typing import List
from dataclasses import dataclass, field


symbol_to_enum_mapping = {
    '(': 'LEFT_PARAN',
    ')': 'RIGHT_PARAN',
    '*': 'STAR',
    '+': 'PLUS',
    '-': 'MINUS',
    '/': 'DIV',
    ';': 'SEMICOLON',
    '<': 'LESS_THAN',
    '>': 'GREATER_THAN',
    '<=': 'LESS_EQUALS',
    '>=': 'GREATER_EQUALS',
    '==': 'EQUALS',
    '!=': 'NOT_EQUALS',
    'not': 'NOT',
    'eof': 'END_OF_FILE',
    'expr': 'EXPRESSION',
    'factor': 'FACTOR',
    'false': 'FALSE_LIT',
    'float': 'FLOAT_LIT',
    'integer': 'INT_LIT',
    'program': 'PROGRAM',
    'stmt': 'STATEMENT',
    'stmts': 'STATEMENTS',
    'sum': 'SUM',
    'term': 'TERM',
    'true': 'TRUE_LIT',
    'relation': 'RELATION',
    'negation': 'NEGATION'
}


def parse_rules(lines: List[str]) -> dict:
    rules = {}
    for line in lines:
        if not line.strip() or line.startswith("#"):
            continue

        if not line.startswith(" "):
            current_rule = line.strip()
            rules[current_rule] = []
            continue

        rules[current_rule].append(line.strip().split())
    return rules


def construct_dfa_states(rules: dict):
    working_rules = {"program": copy.deepcopy(rules["program"])}
    working_rules["program"][0].insert(0, '.')

    states = [State(0, [], closure(rules, working_rules), {})]

    has_changed = True
    while has_changed:
        has_changed = False
        for i in range(len(states)):
            if states[i].expanded:
                continue

            states[i].expanded = True
            transitions = {}
            for rule_name, rule_collection in states[i].rules.items():
                for rule in rule_collection:
                    rule = copy.deepcopy(rule)
                    idx = rule.index('.')
                    rule.remove('.')
                    if idx >= len(rule):
                        continue

                    symbol = rule[idx]
                    if idx + 1 == len(rule):
                        rule.append('.')
                    else:
                        rule.insert(idx + 1, '.')

                    if symbol not in transitions:
                        transitions[symbol] = {}

                    if rule_name not in transitions[symbol]:
                        transitions[symbol][rule_name] = []

                    transitions[symbol][rule_name].append(rule)

            for key, value in transitions.items():
                new_state = State(len(states),
                                  [i], closure(rules, value), {})

                found_idx = None
                for idx, state in enumerate(states):
                    if state == new_state:
                        found_idx = idx
                        state.predecessors.append(i)
                        break

                if found_idx is None:
                    found_idx = len(states)
                    states.append(new_state)
                    has_changed = True

                states[i].successors[key] = found_idx
    return states


def closure(rules, working_rules):
    working_rules = copy.deepcopy(working_rules)
    has_changed = True
    while has_changed:
        has_changed = False
        for rule_collection in list(working_rules.values()):
            for rule in rule_collection:
                idx = rule.index('.')
                if idx + 1 >= len(rule):
                    continue

                symbol = rule[idx + 1]
                if symbol not in rules:
                    continue

                for other_rule in rules[symbol]:
                    if symbol not in working_rules:
                        working_rules[symbol] = []
                    new_rule = ['.']
                    new_rule += other_rule
                    found_it = False
                    for r in working_rules[symbol]:
                        if r == new_rule:
                            found_it = True
                            break
                    if found_it:
                        continue

                    has_changed = True
                    working_rules[symbol].append(new_rule)

    return working_rules


@dataclass
class State:
    index: int
    predecessors: list
    rules: dict
    successors: dict
    expanded: bool = False

    def __ne__(self, other):
        return not self.__eq__(other)

    def __eq__(self, other):
        for key, rules in self.rules.items():
            if key not in other.rules:
                return False
            for r1 in rules:
                found_one = False
                for r2 in other.rules[key]:
                    broke = False
                    for v1, v2 in zip(r1, r2):
                        if v1 != v2:
                            broke = True
                            break
                    if not broke:
                        found_one = True
                if not found_one:
                    return False
        return True


def extract_all_symbols(rules: dict):
    symbols = set(["program"])
    for rule_collection in rules.values():
        for rule in rule_collection:
            for symbol in rule:
                if symbol not in symbols:
                    symbols.add(symbol)
    return symbols


def construct_table(rules: dict, states: []):
    all_symbols = extract_all_symbols(rules)
    non_terminals = set(rules.keys())
    terminals = all_symbols.difference(non_terminals)
    all_symbols = sorted(all_symbols)
    non_terminals = sorted(non_terminals)
    terminals = sorted(terminals)

    table = [[[] for _ in range(len(all_symbols))] for i in range(len(states))]
    for state_index, state in enumerate(states):
        for trans, trans_index in state.successors.items():
            symbol_index = all_symbols.index(trans)
            if trans in terminals:
                table[state_index][symbol_index].append(f"s{trans_index}")
            elif trans in non_terminals:
                table[state_index][symbol_index].append(f"g{trans_index}")

            for rule_name, rule_collection in state.rules.items():
                for rule in rule_collection:
                    if len(rule) > 1 and rule[-2] == '.' and rule[-1] == "eof" and trans == "eof":
                        table[state_index][symbol_index].append("a")

        for rule_name, rule_collection in state.rules.items():
            for rule in rule_collection:
                if rule[-1] == '.':
                    formatted_right_side = ' '.join(
                        filter(lambda s: s != '.', rule))
                    reduce_ = f"r:{rule_name}->{formatted_right_side}"
                    for terminal in terminals:
                        table[state_index][all_symbols.index(
                            terminal)].append(reduce_)

    return all_symbols, table


def create_table_row(row: List[List[str]]) -> str:
    result = "  { "
    result += ", ".join(map(convert_to_state_transitions, row))
    result += " }"
    return result


def convert_to_state_transition(transition: str) -> str:
    t = transition[0]
    rest = transition[1:]
    if t == "r":
        index = rest.index(">")
        rule_symbol = f"GrammarSymbol::{symbol_to_enum_mapping[rest[1:index - 1]]}"
        rest = rest[index + 1:]
        rule = ', '.join(
            map(lambda s: f"GrammarSymbol::{s}",
                map(lambda s: symbol_to_enum_mapping[s], rest.split())))
        return f"{{StateTransitionType::REDUCE, -1, {rule_symbol}, {{{rule}}}}}"
    if t == "s":
        return f"{{StateTransitionType::SHIFT, {rest}}}"
    if t == "g":
        return f"{{StateTransitionType::GOTO, {rest}}}"
    if t == "a":
        return "{StateTransitionType::ACCEPT}"
    return "{}"


def convert_to_state_transitions(elem: List[str]) -> str:
    if not elem:
        return "{}"
    result = "{"
    result += ", ".join(map(convert_to_state_transition, elem))
    result += "}"
    return result


def create_grammar_symbols(header: List[str]) -> str:
    return ",\n".join(map(lambda e: f"  {symbol_to_enum_mapping[e[1]]} = {e[0]}", enumerate(header)))


def create_grammar_symbol_switch_cases(header: List[str]) -> str:
    return "\n".join(
        map(create_switch_case_for_grammar_symbol,
            map(lambda x: symbol_to_enum_mapping[x], header)))


def create_switch_case_for_grammar_symbol(grammar_symbol: str):
    return f"  case {grammar_symbol}:\n    return \"{grammar_symbol}\";"


def main(grammer_file: str = "grammar.txt", header_file: str = "src/Grammar.h", cpp_file: str = "src/Grammar.cpp", verbose: bool = False):
    with open("grammar.txt") as f:
        lines = f.readlines()

    rules = parse_rules(lines)
    if verbose:
        print(rules)
        print()

    states = construct_dfa_states(rules)

    if verbose:
        for s in states:
            print(s)
        print()

    header, table = construct_table(rules, states)

    if verbose:
        for row in [header, *table]:
            print(row)
        print()

    table_content = ",\n".join(map(create_table_row, table))
    grammar_symbols = create_grammar_symbols(header)

    header_template = """#pragma once

#include <vector>
#include <string>

enum GrammarSymbol {
{grammar_symbols}
};

std::string to_string(GrammarSymbol symbol);

enum StateTransitionType {
  SHIFT,
  GOTO,
  REDUCE,
  ACCEPT,
  ERROR
};

std::string to_string(StateTransitionType transition);

struct StateTransition {
  StateTransitionType type = StateTransitionType::ERROR;
  int nextStateIndex = 0;
  GrammarSymbol symbol = END_OF_FILE;
  std::vector<GrammarSymbol> rule = {};
};

std::string to_string(StateTransition &action);

const std::vector<StateTransition> stateTransitionTable[{row_count}][{col_count}] = {
{table_content}
};"""
    header_template = header_template.replace(
        "{grammar_symbols}", grammar_symbols)
    header_template = header_template.replace("{row_count}", str(len(table)))
    header_template = header_template.replace("{col_count}", str(len(header)))
    header_template = header_template.replace("{table_content}", table_content)

    if verbose:
        print(header_template)
    with open(header_file, "w") as f:
        f.write(header_template)

    cpp_template = """
#include "Grammar.h"

std::string to_string(GrammarSymbol symbol) {
  switch (symbol) {
{grammar_symbol_switch_cases}
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
"""
    grammar_symbol_switch_cases = create_grammar_symbol_switch_cases(header)
    cpp_template = cpp_template.replace(
        "{grammar_symbol_switch_cases}", grammar_symbol_switch_cases)

    if verbose:
        print(cpp_template)
    with open(cpp_file, "w") as f:
        f.write(cpp_template)


if __name__ == "__main__":
    main()
