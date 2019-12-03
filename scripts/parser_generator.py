import copy
import logging
import sys
from dataclasses import dataclass
from enum import Enum
from typing import List, Tuple, Set, Dict, Union, Any

LOG = logging.getLogger()
LOG.setLevel(logging.WARN)
handler = logging.StreamHandler(sys.stdout)
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s [%(levelname)s] %(message)s')
handler.setFormatter(formatter)
LOG.addHandler(handler)

END_OF_FILE = "endoffile"

SYMBOL_TO_ENUM_MAPPING = {
    '(': 'LEFT_PARAN',
    ')': 'RIGHT_PARAN',
    '{': 'LEFT_CURLY_BRACE',
    '}': 'RIGHT_CURLY_BRACE',
    '[': 'LEFT_BRACKET',
    ']': 'RIGHT_BRACKET',
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


class Rules:
    def __init__(self):
        self.rules: Dict[str, List[List[str]]] = {}

    def parse_lines(self, lines: List[str]):
        current_rule = ""
        for line in lines:
            if not line.strip() or line.startswith("#"):
                continue

            if not line.startswith(" ") and not line.startswith("\t"):
                current_rule = line.strip()
                self.rules[current_rule] = []
                continue

            if current_rule:
                self.rules[current_rule].append(line.strip().split())
            else:
                LOG.error("Could not associate this line with a rule.")

    def closure(self, working_rules):
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
                    if symbol not in self.rules:
                        continue

                    for other_rule in self.rules[symbol]:
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

    def get_all_symbols(self) -> Set[str]:
        symbols = {"program"}
        for rule_collection in self.rules.values():
            for rule in rule_collection:
                for symbol in rule:
                    if symbol not in symbols:
                        symbols.add(symbol)

        return symbols

    def get_non_terminals(self) -> Set[str]:
        return set(self.rules.keys())

    def print(self):
        for rule in sorted(self.rules):
            LOG.info("Found rule: " + rule + " -> " + str(self.rules[rule]))


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


def construct_dfa_states(rules: Rules) -> List[State]:
    working_rules = {"program": copy.deepcopy(rules.rules["program"])}
    for rule in working_rules["program"]:
        rule.insert(0, '.')

    states = [State(0, [], rules.closure(working_rules), {})]

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
                new_state = State(len(states), [i], rules.closure(value), {})

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


class StateTransitionType(Enum):
    SHIFT = 0
    GOTO = 1
    REDUCE = 2
    ACCEPT = 3
    ERROR = 4


class StateTransition:
    def __init__(self, transition_type: StateTransitionType, transition_index: int = -1, rule_name: str = None,
                 rule: List[str] = None):
        self.type = transition_type
        self.transition_index = transition_index
        self.rule_name = rule_name

        if rule is None:
            rule = []
        self.rule = rule
        if '.' in self.rule:
            self.rule.remove('.')

    def __str__(self):
        return f"ST(type={self.type}, index={self.transition_index}, rule_name={self.rule_name}, rule={self.rule})"

    def __repr__(self):
        return self.__str__()


class StateTransitionTable:
    def __init__(self, header: List[str], table: List[List[List[StateTransition]]]):
        self.header = header
        self.table = table

    def print(self):
        for row in [self.header, *self.table]:
            LOG.debug(str(row))
        LOG.debug("")

    def row_count(self):
        return len(self.table)

    def col_count(self):
        return len(self.header)

    def create_grammar_symbols(self) -> str:
        # TODO rename this method!
        # do NOT sort this! the order matters
        grammar_symbols = enumerate(map(get_grammar_symbol, self.header))
        return ",\n".join(map(lambda e: f"    {e[1]} = {e[0]}", grammar_symbols))

    def get_max_num_rules(self):
        max_num_rules = 0
        for row in self.table:
            for col in row:
                for transition in col:
                    if transition.type != StateTransitionType.REDUCE:
                        continue
                    if len(transition.rule) > max_num_rules:
                        max_num_rules = len(transition.rule)
        return max_num_rules

    def create_init_state_transitions_code(self):
        init_state_transitions_code = ""
        for row_idx, row in enumerate(self.table):
            for col_idx, col in enumerate(row):
                if not col:
                    continue
                transitions = convert_to_state_transitions(col)
                state_transition = f"createStateTransition(&data, {transitions})"
                init_state_transitions_code += f"    stateTransitionTable[{row_idx}][{col_idx}] = {state_transition};\n"
        init_state_transitions_code = init_state_transitions_code[:-1]
        return init_state_transitions_code

    def get_number_of_state_transitions(self):
        num_state_transitions = 0
        num_empty_slots = 0
        for row_idx, row in enumerate(self.table):
            for col_idx, col in enumerate(row):
                num_state_transitions += len(col)
                if not col:
                    num_empty_slots += 1
        return num_state_transitions, num_empty_slots

    def create_grammar_symbol_switch_cases(self) -> str:
        return "\n".join(
            map(create_switch_case_for_grammar_symbol,
                map(get_grammar_symbol, self.header)))


class TableConstructor:
    def __init__(self, rules: Rules, states: List[State] = None):
        if states is None:
            states = []
        self.states = states
        self.non_terminals = rules.get_non_terminals()

        all_symbols_set: Set[str] = rules.get_all_symbols()
        self.terminals = all_symbols_set.difference(self.non_terminals)
        self.terminals = sorted(self.terminals)
        self.non_terminals = sorted(self.non_terminals)
        self.all_symbols: List[str] = sorted(all_symbols_set)

        self.table: List[List[List[StateTransition]]] = [[[] for _ in range(len(self.all_symbols))] for i in
                                                         range(len(states))]

    def add_successors(self, state: State, state_index: int):
        for trans, trans_index in state.successors.items():
            symbol_index = self.all_symbols.index(trans)
            if trans in self.terminals:
                self.table[state_index][symbol_index].append(StateTransition(StateTransitionType.SHIFT, trans_index))
            elif trans in self.non_terminals:
                self.table[state_index][symbol_index].append(StateTransition(StateTransitionType.GOTO, trans_index))

            for rule_name, rule_collection in state.rules.items():
                for rule in rule_collection:
                    if len(rule) > 1 and rule[-2] == '.' and rule[-1] == END_OF_FILE and trans == END_OF_FILE:
                        self.table[state_index][symbol_index].append(StateTransition(StateTransitionType.ACCEPT))

    def add_rules(self, state: State, state_index: int):
        for rule_name, rule_collection in state.rules.items():
            for rule in rule_collection:
                if rule[-1] != '.':
                    continue

                formatted_right_side = ' '.join(filter(lambda s: s != '.', rule))
                transition = StateTransition(StateTransitionType.REDUCE, -1, rule_name, rule)
                for terminal in self.terminals:
                    symbol_index = self.all_symbols.index(terminal)
                    self.table[state_index][symbol_index].append(transition)

    def construct_table(self) -> StateTransitionTable:
        for state_index, state in enumerate(self.states):
            self.add_successors(state, state_index)
            self.add_rules(state, state_index)

        return StateTransitionTable(self.all_symbols, self.table)


class Template:
    def __init__(self, template_str: str):
        self.template_str = template_str

    def finish(self, replacements: Dict[str, Union[str, int, float]]) -> str:
        result = self.template_str
        for repl in replacements:
            result = result.replace("{" + repl + "}", str(replacements[repl]))
        return result


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
            LOG.error(f"Could not find '{symbol}' in the symbol mapping table.")
            exit(1)

    return symbol.upper()


def convert_to_state_transition(transition: StateTransition) -> str:
    if transition.type == StateTransitionType.REDUCE:
        grammar_symbol = get_grammar_symbol(transition.rule_name)
        rule_symbol = f"GrammarSymbol::{grammar_symbol}"
        rule_list = list(map(lambda s: f"GrammarSymbol::{s}", map(get_grammar_symbol, transition.rule)))
        rule = ', '.join(rule_list)
        return f"{{StateTransitionType::REDUCE, -1, {rule_symbol}, {len(transition.rule)}, {{{rule}}}}}"

    if transition.type == StateTransitionType.SHIFT:
        return f"{{StateTransitionType::SHIFT, {transition.transition_index}}}"

    if transition.type == StateTransitionType.GOTO:
        return f"{{StateTransitionType::GOTO, {transition.transition_index}}}"

    if transition.type == StateTransitionType.ACCEPT:
        return "{StateTransitionType::ACCEPT}"

    return "{}"


def convert_to_state_transitions(elem: List[StateTransition]) -> str:
    if not elem:
        return "{}"
    result = "{"
    result += ", ".join(map(convert_to_state_transition, elem))
    result += "}"
    return result


def create_table_content(table: List[List[List[str]]]):
    return ",\n".join(map(create_table_row, table))


def create_switch_case_for_grammar_symbol(grammar_symbol: str):
    return f"    case {grammar_symbol}:\n        return \"{grammar_symbol}\";"


def write_header_file(header_file: str, grammar_symbols_str: str, row_count: int, column_count: int,
                      max_num_rules: int):
    header_template = Template("""#pragma once

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
    ERROR,
};

std::string to_string(StateTransitionType transition);

#define NUM_RULES {max_num_rules}

struct StateTransition {
    StateTransitionType type = StateTransitionType::ERROR;
    int nextStateIndex = 0;
    GrammarSymbol symbol = ENDOFFILE;
    unsigned int numRules = 0;
    GrammarSymbol rule[NUM_RULES] = {};
    StateTransition *next = nullptr;
};

std::string to_string(const StateTransition &action);

void initializeStateTransitionTable();

extern const StateTransition *stateTransitionTable[{row_count}][{col_count}];
""")
    result = header_template.finish({
        "grammar_symbols": grammar_symbols_str,
        "row_count": row_count,
        "col_count": column_count,
        "max_num_rules": max_num_rules
    })

    LOG.debug(result)
    with open(header_file, "w") as f:
        f.write(result)


def write_cpp_file(cpp_file: str, grammar_symbol_switch_cases: str, num_state_transitions: int, row_count: int,
                   col_count: int,
                   init_transition_code: str):
    cpp_template = Template("""#include "Grammar.h"

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

StateTransition* createStateTransition(StateTransition **data, const std::vector<StateTransition> &transitions) {
    StateTransition *current = nullptr;
    for (long i = transitions.size() - 1; i >= 0; i--) {
        auto next = *data;
        (*(char **)data) += sizeof(StateTransition);
        *next = transitions[i];
        next->next = current;
        current = next;
    }
    return current;
}

const StateTransition *stateTransitionTable[{row_count}][{col_count}];

void initializeStateTransitionTable() {
    size_t dataSize = sizeof(StateTransition) * {num_state_transitions};
    auto data = (StateTransition *)malloc(dataSize);
    for (auto &row : stateTransitionTable) {
        for (auto &cell : row) {
            cell = nullptr;
        }
    }

{init_transition_code}
}
""")
    result = cpp_template.finish({
        "grammar_symbol_switch_cases": grammar_symbol_switch_cases,
        "num_state_transitions": num_state_transitions,
        "row_count": row_count,
        "col_count": col_count,
        "init_transition_code": init_transition_code
    })

    LOG.debug(result)
    with open(cpp_file, "w") as f:
        f.write(result)


def main(grammar_file: str = "grammar.txt", header_file: str = "Grammar.h", cpp_file: str = "Grammar.cpp"):
    with open(grammar_file) as f:
        lines = f.readlines()

    rules = Rules()
    rules.parse_lines(lines)
    rules.print()

    # TODO create data structures for header and table, or one for both
    states = construct_dfa_states(rules)

    for s in states:
        LOG.debug(str(s))
    LOG.debug("")

    # TODO create data structures for header and table, or one for both
    table = TableConstructor(rules, states).construct_table()

    table.print()

    # write header file
    row_count = table.row_count()
    col_count = table.col_count()
    grammar_symbols = table.create_grammar_symbols()
    max_num_rules = table.get_max_num_rules()
    write_header_file(header_file, grammar_symbols, row_count, col_count, max_num_rules)

    # write cpp file
    grammar_symbol_switch_cases = table.create_grammar_symbol_switch_cases()
    num_state_transitions, num_empty_slots = table.get_number_of_state_transitions()
    init_state_transitions_code = table.create_init_state_transitions_code()
    write_cpp_file(cpp_file, grammar_symbol_switch_cases, num_state_transitions, row_count, col_count,
                   init_state_transitions_code)

    print_statistics(row_count, col_count, num_empty_slots, num_state_transitions)


def print_statistics(row_count: int, col_count: int, num_empty_slots: int, num_state_transitions: int):
    LOG.info("Generated parser:")
    table_size = row_count * col_count
    size_of_pointer = 4
    table_size_in_kb = table_size * size_of_pointer / 1024
    size_of_state_transition_struct = 64
    state_transitions_size = size_of_state_transition_struct * num_state_transitions / 1024
    LOG.info("  Size of parse table: %dx%d=%d (%.2f KB)", row_count, col_count, table_size, table_size_in_kb)
    LOG.info("  Number of empty table slots: %d", num_empty_slots)
    LOG.info("  Number of state transitions: %d (%d KB)", num_state_transitions, state_transitions_size)


if __name__ == "__main__":
    main()
