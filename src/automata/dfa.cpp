#include "../../include/fa/automata/dfa.hpp"
#include "../../include/fa/automata/fa.hpp"
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

unique_ptr<DFA> DFA::minimize(void) {
  if (!initial_state.has_value() || states.empty())
    return make_unique<DFA>(*this);

  // Initial Partitions
  set<string> finals = final_states;
  set<string> no_finals;
  set_difference(states.begin(), states.end(), finals.begin(), finals.end(),
                 inserter(no_finals, no_finals.begin()));

  vector<set<string>> partitions;
  if (!finals.empty())
    partitions.push_back(finals);
  if (!no_finals.empty())
    partitions.push_back(no_finals);

  vector<char> symbols(alphabet.begin(), alphabet.end());

  while (true) {
    map<string, int> block_of;

    for (size_t i = 0; i < partitions.size(); i++) {
      for (const auto &q : partitions[i]) {
        block_of[q] = i;
      }
    }

    vector<set<string>> new_partition;

    for (const auto &block : partitions) {
      map<vector<int>, set<string>> buckets;

      for (const auto &q : block) {
        vector<int> signature;
        for (char a : symbols) {
          // Asumiendo que DFA es completo. Si no, manejar estados de error.
          string new_state = transitions[q][a];
          signature.push_back(block_of[new_state]);
        }
        buckets[signature].insert(q);
      }
      for (const auto &[sig, states_set] : buckets) {
        new_partition.push_back(states_set);
      }
    }

    if (partitions.size() == new_partition.size())
      break;

    partitions = new_partition;
  }

  // build a new dfa;
  auto min_dfa = make_unique<DFA>();
  map<string, int> state_block;
  for (size_t i = 0; i < partitions.size(); i++) {
    for (const auto &q : partitions[i]) {
      state_block[q] = i;
    }
  }

  for (size_t i = 0; i < partitions.size(); i++) {
    const auto &block = partitions[i];

    bool is_final = false;
    for (const auto &q : block) {
      if (final_states.count(q) > 0) {
        is_final = true;
        break;
      }
    }

    string block_name = "B" + to_string(i);
    min_dfa->add_state(block_name, is_final);
  }

  // initial state
  int initial_block_index = state_block[initial_state.value()];
  string initial_block_name = "B" + to_string(initial_block_index);
  min_dfa->mark_initial_state(initial_block_name);

  // transitions
  for (size_t i = 0; i < partitions.size(); i++) {
    const auto &block = partitions[i];
    string rep = *block.begin();

    for (char a : symbols) {
      string next_state = transitions[rep][a];
      int j = state_block[next_state];

      string from_block = "B" + to_string(i);
      string to_block = "B" + to_string(j);

      min_dfa->add_transition(from_block, a, to_block);
    }
  }

  min_dfa->normalize_states();
  return min_dfa;
}
