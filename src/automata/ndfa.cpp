#include "../../include/fa/automata/ndfa.hpp"
#include "../../include/fa/automata/fa.hpp"
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>

using namespace std;

set<string> NDFA::epsilon_clousure(const set<string> &states) const {
  set<string> closure;
  queue<string> to_process;

  for (const auto &state : states) {
    closure.insert(state);
    to_process.push(state);
  }

  while (!to_process.empty()) {
    string current = to_process.front();
    to_process.pop();

    auto state_it = transitions.find(current);
    if (state_it != transitions.end()) {
      auto symbol_it = state_it->second.find('\0');
      if (symbol_it != state_it->second.end()) {
        for (const auto &next_state : symbol_it->second) {
          if (closure.count(next_state) == 0) {
            closure.insert(next_state);
            to_process.push(next_state);
          }
        }
      }
    }
  }

  return closure;
}

set<string> NDFA::epsilon_clousure(const string &state) const {
  set<string> single_state;
  single_state.insert(state);
  return epsilon_clousure(single_state);
}

set<string> NDFA::move(const set<string> &states, char symbol) const {
  set<string> result;

  for (const auto &state : states) {
    auto state_it = transitions.find(state);
    if (state_it != transitions.end()) {
      auto symbol_it = state_it->second.find(symbol);
      if (symbol_it != state_it->second.end()) {
        for (const auto &next_state : symbol_it->second) {
          result.insert(next_state);
        }
      }
    }
  }
  return result;
}

unique_ptr<DFA> NDFA::determinize(void) {
  auto dfa = make_unique<DFA>();

  set<char> alphabet_without_lambda;
  for (const auto &symbol : alphabet) {
    if (symbol != '\0') {
      alphabet_without_lambda.insert(symbol);
    }
  }

  map<set<string>, string> state_mapping;
  queue<set<string>> to_process;
  int state_counter = 0;

  if (!initial_state.has_value()) {
    throw invalid_argument("ERROR: nullopt in initial_state");
  }

  set<string> inital_clousure = epsilon_clousure(initial_state.value());
  string dfa_initial = "q" + to_string(state_counter++);
  dfa->add_state(dfa_initial);
  state_mapping[inital_clousure] = dfa_initial;
  dfa->mark_initial_state(dfa_initial);
  to_process.push(inital_clousure);

  while (!to_process.empty()) {
    set<string> current_set = to_process.front();
    to_process.pop();

    string current_dfa_state = state_mapping[current_set];

    for (const auto &state : current_set) {
      if (final_states.count(state) > 0) {
        dfa->mark_final_state(current_dfa_state);
        break;
      }
    }

    for (char symbol : alphabet_without_lambda) {
      set<string> next_set = move(current_set, symbol);

      if (next_set.empty())
        continue;

      next_set = epsilon_clousure(next_set);

      if (state_mapping.find(next_set) == state_mapping.end()) {
        string new_dfa_state = "q" + to_string(state_counter++);
        dfa->add_state(new_dfa_state);
        state_mapping[next_set] = new_dfa_state;
        to_process.push(next_set);
      }

      dfa->add_transition(current_dfa_state, symbol, state_mapping[next_set]);
    }
  }

  bool trap_needed = false;
  string q_trap = "q_trap";
  set<string> all_states = dfa->get_states();
  for (const auto &state : all_states) {
    for (char symbol : alphabet_without_lambda) {
      if (!dfa->has_transition(state, symbol)) {
        if (!trap_needed) {
          trap_needed = true;
          dfa->add_state(q_trap);
          for (char sym : alphabet_without_lambda) {
            dfa->add_transition(q_trap, sym, q_trap);
          }
        }
        dfa->add_transition(state, symbol, q_trap);
      }
    }
  }
  return dfa;
}
