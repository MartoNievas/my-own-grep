#include "../../include/fa/regex/regex.hpp"
#include "../../include/fa/automata/dfa.hpp"
#include "../../include/fa/automata/ndfa.hpp"
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace std;

namespace fa::regex {

Regex::~Regex() = default;

const DFA *Regex::dfa() const {
  if (_dfa_cache == nullptr) {
    unique_ptr<NDFA> ndfa = to_ndfa();
    unique_ptr<DFA> dfa_det(ndfa->determinize());
    _dfa_cache = dfa_det->minimize();
  }
  return _dfa_cache.get();
}

static unique_ptr<DFA_Fast> build_fast_dfa(const DFA &dfa) {
  auto fast = make_unique<DFA_Fast>();

  const auto &initial_opt = dfa.get_inital_state();
  if (!initial_opt.has_value())
    return fast;

  unordered_map<string, int> state_index;
  int idx = 0;
  state_index[initial_opt.value()] = idx++;

  const auto &trans = dfa.get_transitions();
  for (const auto &[state, edges] : trans) {
    if (!state_index.count(state))
      state_index[state] = idx++;
    for (const auto &[symbol, dst] : edges) {
      if (!state_index.count(dst))
        state_index[dst] = idx++;
    }
  }

  fast->initial_state = 0;

  fast->transitions.assign(idx, {});
  for (auto &row : fast->transitions)
    row.fill(-1);

  // Llenar transiciones (FUERA del loop anterior)
  for (const auto &[state, edges] : trans) {
    int from = state_index[state];
    for (const auto &[symbol, dst] : edges) {
      fast->transitions[from][(unsigned char)symbol] = state_index[dst];
    }
  }

  fast->accept_states.assign(idx, false);
  for (const auto &accept : dfa.get_final_states()) {
    auto it = state_index.find(accept);
    if (it != state_index.end())
      fast->accept_states[it->second] = true;
  }

  return fast;
}

bool Regex::match(string_view word) const {
  if (!_dfa_fast_cache) { // <-- ! aquí
    const DFA *dfa_ptr = dfa();
    if (!dfa_ptr)
      return false;
    if (!dfa_ptr->get_inital_state().has_value())
      return false;
    _dfa_fast_cache = build_fast_dfa(*dfa_ptr);
    if (!_dfa_fast_cache || _dfa_fast_cache->initial_state < 0)
      return false;
  }

  const DFA_Fast &fast = *_dfa_fast_cache;
  int curr = fast.initial_state;

  for (unsigned char symbol : word) {
    curr = fast.transitions[curr][symbol];
    if (curr < 0)
      return false;
  }

  return fast.accept_states[curr];
}

/* EMPTY */

unique_ptr<NDFA> Empty::to_ndfa(void) const {
  auto res = make_unique<NDFA>();

  res->add_state("q0", false);
  res->mark_initial_state("q0");
  return res;
}

bool Empty::_atomic(void) const { return true; }

string Empty::to_string(void) const { return "∅"; }

/* LAMBDA */

unique_ptr<NDFA> Lambda::to_ndfa(void) const {
  auto res = make_unique<NDFA>();
  res->add_state("q0", true);
  res->mark_initial_state("q0");
  return res;
}

bool Lambda::_atomic(void) const { return true; }

string Lambda::to_string(void) const { return "λ"; }

/* CHAR */
Char::Char(char c) : symbol(c) {}

unique_ptr<NDFA> Char::to_ndfa(void) const {
  auto res = make_unique<NDFA>();
  res->add_state("q0");
  res->add_state("q1", true);
  res->add_transition("q0", symbol, "q1");
  res->mark_initial_state("q0");
  return res;
}

bool Char::_atomic(void) const { return true; }

string Char::to_string(void) const {
  if (symbol == '\0') {
    return "λ";
  }
  return string(1, symbol);
}

/* CONCAT */

Concat::Concat(shared_ptr<Regex> e1, shared_ptr<Regex> e2)
    : expr1(e1), expr2(e2) {}

static void cpy_states_concat(const unique_ptr<NDFA> &src, NDFA *dst,
                              const string &prefix) {
  if (!src || !dst)
    return;

  if (prefix == "EXPR1_") {
    for (const auto &state : src->get_states()) {
      dst->add_state(prefix + state, false);
    }
  } else if (prefix == "EXPR2_") {
    set<string> final_states = src->get_final_states();
    for (const auto &state : src->get_states()) {
      bool is_final = final_states.find(state) != final_states.end();
      dst->add_state(prefix + state, is_final);
    }
  }
}

static void cpy_transitions(const unique_ptr<NDFA> &src, NDFA *dst,
                            const string &prefix) {
  if (!src || !dst)
    return;

  for (const auto &[state, trans] : src->get_transitions()) {
    for (const auto &[symbol, to_states] : trans) {
      for (const string &to_state : to_states) {
        dst->add_transition(prefix + state, symbol, prefix + to_state);
      }
    }
  }
}

unique_ptr<NDFA> Concat::to_ndfa(void) const {
  unique_ptr<NDFA> ndfa_expr1 = expr1->to_ndfa();
  unique_ptr<NDFA> ndfa_expr2 = expr2->to_ndfa();

  if (!ndfa_expr1 || !ndfa_expr2) {
    return nullptr;
  }
  if (!ndfa_expr1->get_inital_state() || !ndfa_expr2->get_inital_state()) {
    return nullptr;
  }

  auto new_ndfa = make_unique<NDFA>();

  /* Copy states and transitions */
  cpy_states_concat(ndfa_expr1, new_ndfa.get(), "EXPR1_");
  cpy_states_concat(ndfa_expr2, new_ndfa.get(), "EXPR2_");
  cpy_transitions(ndfa_expr1, new_ndfa.get(), "EXPR1_");
  cpy_transitions(ndfa_expr2, new_ndfa.get(), "EXPR2_");

  /*New initial state for new_ndfa*/
  new_ndfa->mark_initial_state("EXPR1_" +
                               ndfa_expr1->get_inital_state().value());

  string initial_expr2_new = "EXPR2_" + ndfa_expr2->get_inital_state().value();
  for (const auto &final_state : ndfa_expr1->get_final_states()) {
    new_ndfa->add_transition("EXPR1_" + final_state, '\0', initial_expr2_new);
    /* \0 represents epsilon or lambda transition */
  }

  return new_ndfa;
}

bool Concat::_atomic(void) const { return false; }

string Concat::to_string(void) const {
  string s1 =
      expr1->_atomic() ? expr1->to_string() : "(" + expr1->to_string() + ")";
  string s2 =
      expr2->_atomic() ? expr2->to_string() : "(" + expr2->to_string() + ")";
  return s1 + s2;
}

/* UNION */

Union::Union(shared_ptr<Regex> e1, shared_ptr<Regex> e2)
    : expr1(e1), expr2(e2) {}

static void cpy_states(const unique_ptr<NDFA> &src, NDFA *dst,
                       const string &prefix) {
  if (!src || !dst)
    return;

  for (const auto &state : src->get_states()) {
    dst->add_state(prefix + state, false);
  }
}

unique_ptr<NDFA> Union::to_ndfa(void) const {
  if (!expr1 || !expr2)
    return nullptr;
  unique_ptr<NDFA> ndfa_expr1 = expr1->to_ndfa();
  unique_ptr<NDFA> ndfa_expr2 = expr2->to_ndfa();

  if (!ndfa_expr1 || !ndfa_expr2) {
    return nullptr;
  }

  if (!ndfa_expr1->get_inital_state() || !ndfa_expr2->get_inital_state()) {
    return nullptr;
  }

  auto new_ndfa = make_unique<NDFA>();

  cpy_states(ndfa_expr1, new_ndfa.get(), "EXPR1_");
  cpy_states(ndfa_expr2, new_ndfa.get(), "EXPR2_");
  cpy_transitions(ndfa_expr1, new_ndfa.get(), "EXPR1_");
  cpy_transitions(ndfa_expr2, new_ndfa.get(), "EXPR2_");

  string new_initial_state = "q0";
  string new_final_state = "qf";

  new_ndfa->add_state(new_initial_state);
  new_ndfa->add_state(new_final_state, true);
  new_ndfa->mark_initial_state(new_initial_state);

  new_ndfa->add_transition(new_initial_state, '\0',
                           "EXPR1_" + ndfa_expr1->get_inital_state().value());
  new_ndfa->add_transition(new_initial_state, '\0',
                           "EXPR2_" + ndfa_expr2->get_inital_state().value());

  for (auto const &final_state : ndfa_expr1->get_final_states()) {
    new_ndfa->add_transition("EXPR1_" + final_state, '\0', new_final_state);
  }

  for (auto const &final_state : ndfa_expr2->get_final_states()) {
    new_ndfa->add_transition("EXPR2_" + final_state, '\0', new_final_state);
  }

  return new_ndfa;
}

bool Union::_atomic(void) const { return false; }

string Union::to_string(void) const {
  string s1 =
      expr1->_atomic() ? expr1->to_string() : "(" + expr1->to_string() + ")";
  string s2 =
      expr2->_atomic() ? expr2->to_string() : "(" + expr2->to_string() + ")";
  return s1 + "|" + s2;
}

/* STAR */

Star::Star(shared_ptr<Regex> e) : expr(e) {}

unique_ptr<NDFA> Star::to_ndfa(void) const {
  unique_ptr<NDFA> ndfa_expr = expr->to_ndfa();
  if (!ndfa_expr)
    return nullptr;

  auto init_opt = ndfa_expr->get_inital_state();
  if (!init_opt) {
    return nullptr;
  }

  auto new_ndfa = make_unique<NDFA>();
  new_ndfa->add_state("q0", false);
  new_ndfa->add_state("qf", true);
  new_ndfa->mark_initial_state("q0");

  cpy_states(ndfa_expr, new_ndfa.get(), "EXPR_");
  cpy_transitions(ndfa_expr, new_ndfa.get(), "EXPR_");

  string prefixed_inner_init = "EXPR_" + init_opt.value();

  new_ndfa->add_transition("q0", '\0', "qf");
  new_ndfa->add_transition("q0", '\0', prefixed_inner_init);

  for (const auto &final_state : ndfa_expr->get_final_states()) {
    string prefixed_f = "EXPR_" + final_state;
    new_ndfa->add_transition(prefixed_f, '\0', "qf");
    new_ndfa->add_transition(prefixed_f, '\0', prefixed_inner_init);
  }

  return new_ndfa;
}

bool Star::_atomic(void) const { return false; }

string Star::to_string(void) const {
  return (expr->_atomic()) ? expr->to_string() + "*"
                           : "(" + expr->to_string() + ")*";
}

/* PLUS */

Plus::Plus(shared_ptr<Regex> e) : expr(e) {}

unique_ptr<NDFA> Plus::to_ndfa(void) const {
  unique_ptr<NDFA> ndfa_expr = expr->to_ndfa();
  if (!ndfa_expr)
    return nullptr;

  auto init_opt = ndfa_expr->get_inital_state();
  if (!init_opt) {
    return nullptr;
  }

  auto new_ndfa = make_unique<NDFA>();
  new_ndfa->add_state("q0", false);
  new_ndfa->add_state("qf", true);
  new_ndfa->mark_initial_state("q0");

  cpy_states(ndfa_expr, new_ndfa.get(), "EXPR_");
  cpy_transitions(ndfa_expr, new_ndfa.get(), "EXPR_");

  string prefixed_inner_init = "EXPR_" + init_opt.value();

  new_ndfa->add_transition("q0", '\0', prefixed_inner_init);

  for (const auto &final_state : ndfa_expr->get_final_states()) {
    string prefixed_f = "EXPR_" + final_state;
    new_ndfa->add_transition(prefixed_f, '\0', "qf");
    new_ndfa->add_transition(prefixed_f, '\0', prefixed_inner_init);
  }

  return new_ndfa;
}

bool Plus::_atomic(void) const { return false; }

string Plus::to_string(void) const {
  return (expr->_atomic()) ? expr->to_string() + "+"
                           : "(" + expr->to_string() + ")+";
}

} // namespace fa::regex
