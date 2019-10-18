import copy
from typing import List
from dataclasses import dataclass

END_OF_FILE = "endoffile"

SYMBOL_TO_ENUM_MAPPING = {
    '(': 'LEFT_PARAN',
    ')': 'RIGHT_PARAN',
    '{': 'LEFT_CURLY_BRACE',
    '}': 'RIGHT_CURLY_BRACE',
    '*': 'STAR',
    '+': 'PLUS',
    '-': 'MINUS',
    '/': 'DIV',
    '=': 'SINGLE_EQUALS',
    ',': 'COMMA',
    ';': 'SEMICOLON',
    '<': 'LESS_THAN',
    '>': 'GREATER_THAN',
    '<=': 'LESS_EQUALS',
    '>=': 'GREATER_EQUALS',
    '==': 'DOUBLE_EQUALS',
    '!=': 'NOT_EQUALS',
}


def parse_rules(lines: List[str]) -> dict:
    rules = {}
    current_rule = ""
    for line in lines:
        if not line.strip() or line.startswith("#"):
            continue

        if not line.startswith(" "):
            current_rule = line.strip()
            rules[current_rule] = []
            continue

        if current_rule:
            rules[current_rule].append(line.strip().split())
        else:
            print("Could not associate this line with a rule.")

    return rules


def construct_dfa_states(rules: dict):
    working_rules = {"program": copy.deepcopy(rules["program"])}
    for rule in working_rules["program"]:
        rule.insert(0, '.')

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
    symbols = {"program"}
    for rule_collection in rules.values():
        for rule in rule_collection:
            for symbol in rule:
                if symbol not in symbols:
                    symbols.add(symbol)

    return symbols


class TableConstructor:
    def __init__(self, rules: dict, states: List[State] = None):
        if states is None:
            states = []
        self.states = states
        self.all_symbols = extract_all_symbols(rules)
        self.non_terminals = set(rules.keys())
        self.terminals = self.all_symbols.difference(self.non_terminals)
        self.all_symbols = sorted(self.all_symbols)
        self.non_terminals = sorted(self.non_terminals)
        self.terminals = sorted(self.terminals)

        self.table = [[[] for _ in range(len(self.all_symbols))] for i in range(len(states))]

    def add_successors(self, state: State, state_index: int):
        for trans, trans_index in state.successors.items():
            symbol_index = self.all_symbols.index(trans)
            if trans in self.terminals:
                self.table[state_index][symbol_index].append(f"s{trans_index}")
            elif trans in self.non_terminals:
                self.table[state_index][symbol_index].append(f"g{trans_index}")

            for rule_name, rule_collection in state.rules.items():
                for rule in rule_collection:
                    if len(rule) > 1 and rule[-2] == '.' and rule[-1] == END_OF_FILE and trans == END_OF_FILE:
                        self.table[state_index][symbol_index].append("a")

    def add_rules(self, state: State, state_index: int):
        for rule_name, rule_collection in state.rules.items():
            for rule in rule_collection:
                if rule[-1] != '.':
                    continue

                formatted_right_side = ' '.join(filter(lambda s: s != '.', rule))
                reduce_ = f"r:{rule_name}->{formatted_right_side}"
                for terminal in self.terminals:
                    symbol_index = self.all_symbols.index(terminal)
                    self.table[state_index][symbol_index].append(reduce_)

    def construct_table(self):
        for state_index, state in enumerate(self.states):
            self.add_successors(state, state_index)
            self.add_rules(state, state_index)

        return self.all_symbols, self.table


def create_table_row(row: List[List[str]]) -> str:
    result = "  { "
    result += ", ".join(map(convert_to_state_transitions, row))
    result += " }"
    return result


def get_grammar_symbol(symbol):
    found_other_char = False
    for c in symbol:
        if not c.isalpha() and not c == "_":
            found_other_char = True

    if found_other_char:
        if symbol in SYMBOL_TO_ENUM_MAPPING:
            return SYMBOL_TO_ENUM_MAPPING[symbol]
        else:
            print(f"Could not find '{symbol}' in the symbol mapping table.")
            exit(1)

    return symbol.upper()


def convert_to_state_transition(transition: str) -> str:
    t = transition[0]
    rest = transition[1:]

    if t == "r":
        index = rest.index(">")
        grammar_symbol = get_grammar_symbol(rest[1:index - 1])
        rule_symbol = f"GrammarSymbol::{grammar_symbol}"
        rest = rest[index + 1:]
        rule = ', '.join(
            map(lambda s: f"GrammarSymbol::{s}",
                map(get_grammar_symbol, rest.split())))
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
    return ",\n".join(map(lambda e: f"  {get_grammar_symbol(e[1])} = {e[0]}", enumerate(header)))


def create_grammar_symbol_switch_cases(header: List[str]) -> str:
    return "\n".join(
        map(create_switch_case_for_grammar_symbol,
            map(get_grammar_symbol, header)))


def create_switch_case_for_grammar_symbol(grammar_symbol: str):
    return f"    case {grammar_symbol}:\n        return \"{grammar_symbol}\";"


def main(grammar_file: str = "grammar.txt", header_file: str = "Grammar.h", cpp_file: str = "Grammar.cpp",
         verbose: bool = False):
    with open(grammar_file) as f:
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

    header, table = TableConstructor(rules, states).construct_table()

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
  GrammarSymbol symbol = ENDOFFILE;
  std::vector<GrammarSymbol> rule = {};
};

std::string to_string(const StateTransition &action);

const std::vector<StateTransition> stateTransitionTable[{row_count}][{col_count}] = {
{table_content}
};
"""
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
    default:
        return "ERROR";
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
    default:
        return "ERROR";
    }
}

std::string to_string(const StateTransition &action) {
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
