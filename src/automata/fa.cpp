#include "fa.hpp"
#include <format>
#include <map>
#include <optional>
#include <stdexcept>

FA::FA() : initial_state(std::nullopt) {}

void FA::add_state(const std::string &state, bool final) {
  if (states.contains(state)) {
    throw std::invalid_argument(
        std::format("The state {} already belongs to the automaton", state));
  }
  states.insert(state);
  if (final)
    final_states.insert(state);
}

void FA::mark_initial_state(const std::string &state) {
  if (!states.contains(state)) {
    throw std::invalid_argument(
        std::format("The state: {} does not belong to the automaton", state));
  }
  initial_state = state;
}

int FA::size() const { return static_cast<int>(states.size()); }

FA &FA::normalize_states(void) {
  std::map<std::string, std::string> new_names = {};

  if (initial_state) {
    new_names[*initial_state] = "q0";
  }
  int i = 1;
  for (const auto &state : states) {
    if (!initial_state || state != *initial_state) {
      new_names[state] = std::format("q{}", i++);
    }
  }

  for (const auto &[old_name, new_name] : new_names) {
    _rename_state(old_name, "temp_" + new_name);
  }

  for (const auto &[old_name, new_name] : new_names) {
    _rename_state("temp_" + new_name, new_name);
  }

  return *this;
}

void FA::_rename_state(const std::string &old_name,
                       const std::string &new_name) {
  if (old_name == new_name)
    return;

  if (states.contains(old_name)) {
    states.erase(old_name);
    states.insert(new_name);
  }

  if (initial_state && *initial_state == old_name) {
    initial_state = new_name;
  }

  if (final_states.contains(old_name)) {
    final_states.erase(old_name);
    final_states.insert(new_name);
  }

  if (transitions.contains(old_name)) {
    auto nh = transitions.extract(old_name);
    nh.key() = new_name;
    transitions.insert(std::move(nh));
  }

  for (auto &[source, symbol_map] : transitions) {
    for (auto &[symbol, target_set] : symbol_map) {
      if (target_set.contains(old_name)) {
        target_set.erase(old_name);
        target_set.insert(new_name);
      }
    }
  }
}
