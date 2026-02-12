#include "../../include/fa/automata/ndfa.hpp"
#include "../../include/fa/automata/dfa.hpp"
#include <map>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>

using namespace std;

set<string> NDFA::epsilon_closure(const set<string> &states) const {
  set<string> closure(states);
  queue<string> to_process;
  for (const auto &s : states)
    to_process.push(s);

  while (!to_process.empty()) {
    string current = to_process.front();
    to_process.pop();
    auto state_it = transitions.find(current);
    if (state_it == transitions.end())
      continue;
    auto symbol_it = state_it->second.find(EPSILON);
    if (symbol_it == state_it->second.end())
      continue;
    for (const auto &next : symbol_it->second)
      if (closure.insert(next).second)
        to_process.push(next);
  }
  return closure;
}

set<string> NDFA::epsilon_closure(const string &state) const {
  return epsilon_closure(set<string>{state});
}

set<string> NDFA::move(const set<string> &states, char symbol) const {
  set<string> result;
  for (const auto &state : states) {
    auto state_it = transitions.find(state);
    if (state_it == transitions.end())
      continue;
    auto symbol_it = state_it->second.find(symbol);
    if (symbol_it == state_it->second.end())
      continue;
    result.insert(symbol_it->second.begin(), symbol_it->second.end());
  }
  return result;
}

unique_ptr<DFA> NDFA::determinize() const {
  if (!initial_state.has_value())
    throw invalid_argument("NDFA initial state is not set");

  set<char> alphabet_clean;
  for (char s : alphabet)
    if (s != EPSILON)
      alphabet_clean.insert(s);

  auto dfa = make_unique<DFA>();
  map<set<string>, string> state_mapping;
  queue<set<string>> to_process;
  int counter = 0;

  auto new_dfa_state = [&](const set<string> &ndfa_set) -> string {
    string name = "q" + to_string(counter++);
    dfa->add_state(name);
    state_mapping[ndfa_set] = name;
    to_process.push(ndfa_set);
    return name;
  };

  set<string> initial_set = epsilon_closure(initial_state.value());
  string dfa_initial = new_dfa_state(initial_set);
  dfa->mark_initial_state(dfa_initial);

  while (!to_process.empty()) {
    set<string> current_set = to_process.front();
    to_process.pop();
    const string &current_name = state_mapping[current_set];

    for (const auto &s : current_set)
      if (final_states.contains(s)) {
        dfa->mark_final_state(current_name);
        break;
      }

    for (char symbol : alphabet_clean) {
      set<string> next_set = epsilon_closure(move(current_set, symbol));
      if (next_set.empty())
        continue;
      if (!state_mapping.contains(next_set))
        new_dfa_state(next_set);
      dfa->add_transition(current_name, symbol, state_mapping[next_set]);
    }
  }

  bool trap_created = false;
  string q_trap = "q_trap";
  for (const auto &state : dfa->get_states()) {
    for (char symbol : alphabet_clean) {
      if (dfa->has_transition(state, symbol))
        continue;
      if (!trap_created) {
        dfa->add_state(q_trap);
        for (char s : alphabet_clean)
          dfa->add_transition(q_trap, s, q_trap);
        trap_created = true;
      }
      dfa->add_transition(state, symbol, q_trap);
    }
  }

  return dfa;
}
