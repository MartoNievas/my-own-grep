#include "../../include/fa/automata/dfa.hpp"
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

using namespace std;

unique_ptr<DFA> DFA::minimize(void) {
  if (!initial_state.has_value() || states.empty())
    return make_unique<DFA>(*this);

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
  map<string, int> block_of;

  while (true) {
    block_of.clear();
    for (size_t i = 0; i < partitions.size(); i++)
      for (const auto &q : partitions[i])
        block_of[q] = i;

    vector<set<string>> new_partitions;
    for (const auto &block : partitions) {
      map<vector<int>, set<string>> buckets;
      for (const auto &q : block) {
        vector<int> sig;
        sig.reserve(symbols.size());
        for (char a : symbols)
          sig.push_back(block_of[transitions[q][a]]);
        buckets[sig].insert(q);
      }
      for (auto &[_, group] : buckets)
        new_partitions.push_back(move(group));
    }

    if (new_partitions.size() == partitions.size())
      break;
    partitions = move(new_partitions);
  }

  // block_of ya tiene el resultado final, no hay que reconstruirlo
  auto block_name = [](int i) { return "B" + to_string(i); };
  auto min_dfa = make_unique<DFA>();

  for (size_t i = 0; i < partitions.size(); i++) {
    bool is_final =
        any_of(partitions[i].begin(), partitions[i].end(),
               [&](const string &q) { return final_states.count(q); });
    min_dfa->add_state(block_name(i), is_final);
  }

  min_dfa->mark_initial_state(block_name(block_of[initial_state.value()]));

  for (size_t i = 0; i < partitions.size(); i++) {
    const string &rep = *partitions[i].begin();
    for (char a : symbols)
      min_dfa->add_transition(block_name(i), a,
                              block_name(block_of[transitions[rep][a]]));
  }

  min_dfa->normalize_states();
  return min_dfa;
}
