#ifndef FA_HPP
#define FA_HPP

#include <algorithm>
#include <format>
#include <iomanip>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

const char EPSILON = '\0';

template <typename T> class FA {
protected:
  std::set<std::string> states;
  std::optional<std::string> initial_state;
  std::map<std::string, std::map<char, T>> transitions;
  std::set<std::string> final_states;
  std::set<char> alphabet;

public:
  FA() : initial_state(std::nullopt) {}
  virtual ~FA() = default;

  std::set<std::string> &get_states(void) { return states; }

  bool has_transition(const std::string &s1, char symbol) {
    if (transitions.find(s1) == transitions.end()) {
      return false;
    }
    if (transitions.at(s1).find(symbol) == transitions.at(s1).end()) {
      return false;
    }
    if constexpr (std::is_same_v<T, std::set<std::string>>) {
      return !transitions.at(s1).at(symbol).empty();
    } else {
      return !transitions.at(s1).at(symbol).empty();
    }
  }

  void add_state(const std::string &state, bool final = false) {
    if (states.contains(state)) {
      throw std::invalid_argument(
          std::format("The state {} already belongs to the automaton", state));
    }
    states.insert(state);
    if (final)
      final_states.insert(state);
  }

  void mark_initial_state(const std::string &state) {
    if (!states.contains(state)) {
      throw std::invalid_argument(
          std::format("The state: {} does not belong to the automaton", state));
    }
    initial_state = state;
  }

  void mark_final_state(const std::string &state) {
    if (!states.contains(state)) {
      throw std::invalid_argument(
          std::format("The state: {} does not belong to the automaton", state));
    }
    if (!final_states.contains(state)) {
      final_states.insert(state);
    }
  }

  void add_transition(const std::string &from, char symbol,
                      const std::string &to) {
    if (!states.contains(from) || !states.contains(to))
      return;

    if (symbol != EPSILON)
      alphabet.insert(symbol);

    if constexpr (std::is_same_v<T, std::set<std::string>>) {
      transitions[from][symbol].insert(to);
    } else {
      transitions[from][symbol] = to;
    }
  }

  int size() const { return static_cast<int>(states.size()); }

  FA &normalize_states(void) {
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

  std::string transitions_table(void) const {
    std::stringstream ss;
    std::vector<char> sorted_alphabet(alphabet.begin(), alphabet.end());

    if constexpr (std::is_same_v<T, std::set<std::string>>) {
      sorted_alphabet.insert(sorted_alphabet.begin(), EPSILON);
    }

    const int col_w = 10;
    ss << std::left << std::setw(12) << "Estado" << " | ";
    for (char c : sorted_alphabet) {
      ss << std::setw(col_w) << (c == EPSILON ? "EPS" : std::string(1, c))
         << " | ";
    }
    ss << "\n" << std::string(15 + col_w * sorted_alphabet.size(), '-') << "\n";

    for (const auto &state : states) {
      std::string label =
          (initial_state && state == *initial_state) ? "->" : "";
      label += state + (final_states.contains(state) ? "*" : "");

      ss << std::left << std::setw(12) << label << " | ";

      for (char c : sorted_alphabet) {
        std::string cell = "-";
        if (transitions.contains(state) && transitions.at(state).contains(c)) {
          const auto &target = transitions.at(state).at(c);
          if constexpr (std::is_same_v<T, std::set<std::string>>) {
            cell = "{";
            for (const auto &t : target)
              cell += t + ",";
            if (cell.size() > 1)
              cell.pop_back();
            cell += "}";
          } else {
            cell = target;
          }
        }
        ss << std::setw(col_w) << cell << " | ";
      }
      ss << "\n";
    }
    return ss.str();
  }

protected:
  void _rename_state(const std::string &old_name, const std::string &new_name) {
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
      for (auto &[symbol, target] : symbol_map) {
        if constexpr (std::is_same_v<T, std::set<std::string>>) {
          if (target.contains(old_name)) {
            target.erase(old_name);
            target.insert(new_name);
          }
        } else {
          if (target == old_name) {
            target = new_name;
          }
        }
      }
    }
  }
};

#endif
