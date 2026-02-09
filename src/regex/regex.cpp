#include "../../include/fa/regex/regex.hpp"
#include "../../include/fa/automata/dfa.hpp"
#include "../../include/fa/automata/ndfa.hpp"
#include <memory>
#include <optional>
#include <string>
using namespace std;

namespace fa::regex {

Regex::~Regex() = default;

DFA *Regex::dfa() const {
  if (_dfa_cache == nullptr) {
    NDFA *ndfa = to_ndfa();
    DFA *dfa = ndfa->determinize();
    _dfa_cache = std::unique_ptr<DFA>(dfa->minimize());
    delete ndfa;
    delete dfa;
  }
  return _dfa_cache.get();
}

bool Regex::match(const string &word) const {
  DFA *dfa_to_match = dfa();

  if (!dfa_to_match->get_inital_state().has_value())
    return false;

  string curr = dfa_to_match->get_inital_state().value();

  for (char symbol : word) {
    auto &trans = dfa_to_match->get_transitions();

    if (trans.find(curr) == trans.end()) {
      return false;
    }

    if (trans.at(curr).find(symbol) == trans.at(curr).end()) {
      return false;
    }

    curr = trans.at(curr).at(symbol);
  }

  return dfa_to_match->get_final_states().count(curr) > 0;
}

/* EMPTY */

NDFA *Empty::to_ndfa(void) const {
  NDFA *res = new NDFA();

  res->add_state("q0", false);
  res->mark_initial_state("q0");
  return res;
}

bool Empty::_atomic(void) const { return true; }

string Empty::to_string(void) const { return "∅"; }

/*LAMBDA*/

NDFA *Lambda::to_ndfa(void) const {
  NDFA *res = new NDFA();
  res->add_state("q0", true);
  res->mark_initial_state("q0");
  return res;
}

bool Lambda::_atomic(void) const { return true; }

string Lambda::to_string(void) const { return "λ"; }

/*CHAR*/
Char::Char(char c) : symbol(c) {}

NDFA *Char::to_ndfa(void) const {
  NDFA *res = new NDFA();
  res->add_state("q0");
  res->add_state("q1", true);
  res->add_transition("q0", symbol, "q1");
  res->mark_initial_state("q0");
  return res;
}

bool Char::_atomic(void) const { return true; }

string Char::to_string(void) const {
  if (symbol == '\0') {
    return "ε"; // o "λ"
  }
  return string(1, symbol);
}

/*CONCAT*/

Concat::Concat(shared_ptr<Regex> e1, shared_ptr<Regex> e2)
    : expr1(e1), expr2(e2) {};

static void cpy_states_concat(NDFA *src, NDFA *dst, const string &prefix) {
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

static void cpy_transitions(NDFA *src, NDFA *dst, const string &prefix) {
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

NDFA *Concat::to_ndfa(void) const {

  NDFA *ndfa_expr1 = expr1->to_ndfa();
  NDFA *ndfa_expr2 = expr2->to_ndfa();
  if (!ndfa_expr1 || !ndfa_expr2) {
    delete ndfa_expr1;
    delete ndfa_expr2;
    return nullptr;
  }
  if (!ndfa_expr1->get_inital_state() || !ndfa_expr2->get_inital_state()) {
    delete ndfa_expr1;
    delete ndfa_expr2;
    return nullptr;
  }
  NDFA *new_ndfa = new NDFA();

  /*Copy states and transitions*/
  cpy_states_concat(ndfa_expr1, new_ndfa, "EXPR1_");
  cpy_states_concat(ndfa_expr2, new_ndfa, "EXPR2_");
  cpy_transitions(ndfa_expr1, new_ndfa, "EXPR1_");
  cpy_transitions(ndfa_expr2, new_ndfa, "EXPR2_");

  /*New initial state for new_ndfa*/
  new_ndfa->mark_initial_state("EXPR1_" +
                               ndfa_expr1->get_inital_state().value());

  string initial_expr2_new = "EXPR2_" + ndfa_expr2->get_inital_state().value();
  for (const auto &final_state : ndfa_expr1->get_final_states()) {
    new_ndfa->add_transition("EXPR1_" + final_state, '\0', initial_expr2_new);
    /* \0 represents epsilon or lambda transition*/
  }
  delete ndfa_expr1;
  delete ndfa_expr2;
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

Union::Union(shared_ptr<Regex> e1, shared_ptr<Regex> e2)
    : expr1(e1), expr2(e2) {};

static void cpy_states(NDFA *src, NDFA *dst, const string &prefix) {
  if (!src || !dst)
    return;

  for (const auto &state : src->get_states()) {
    dst->add_state(prefix + state, false);
  }
}

NDFA *Union::to_ndfa(void) const {
  if (!expr1 || !expr2)
    return nullptr;
  NDFA *ndfa_expr1 = expr1->to_ndfa();
  NDFA *ndfa_expr2 = expr2->to_ndfa();

  if (!ndfa_expr1 || !ndfa_expr2) {
    delete ndfa_expr1;
    delete ndfa_expr2;
    return nullptr;
  }

  if (!ndfa_expr1->get_inital_state() || !ndfa_expr2->get_inital_state()) {
    delete ndfa_expr1;
    delete ndfa_expr2;
    return nullptr;
  }

  NDFA *new_ndfa = new NDFA();

  cpy_states(ndfa_expr1, new_ndfa, "EXPR1_");
  cpy_states(ndfa_expr2, new_ndfa, "EXPR2_");
  cpy_transitions(ndfa_expr1, new_ndfa, "EXPR1_");
  cpy_transitions(ndfa_expr2, new_ndfa, "EXPR2_");

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

  delete ndfa_expr1;
  delete ndfa_expr2;
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

Star::Star(shared_ptr<Regex> e) : expr(e) {};

NDFA *Star::to_ndfa(void) const {
  NDFA *ndfa_expr = expr->to_ndfa();
  if (!ndfa_expr)
    return nullptr;

  auto init_opt = ndfa_expr->get_inital_state();
  if (!init_opt) {
    delete ndfa_expr;
    return nullptr;
  }

  NDFA *new_ndfa = new NDFA();
  new_ndfa->add_state("q0", false);
  new_ndfa->add_state("qf", true);
  new_ndfa->mark_initial_state("q0");

  cpy_states(ndfa_expr, new_ndfa, "EXPR_");
  cpy_transitions(ndfa_expr, new_ndfa, "EXPR_");

  string prefixed_inner_init = "EXPR_" + init_opt.value();

  new_ndfa->add_transition("q0", '\0', "qf");
  new_ndfa->add_transition("q0", '\0', prefixed_inner_init);

  for (const auto &final_state : ndfa_expr->get_final_states()) {
    string prefixed_f = "EXPR_" + final_state;
    new_ndfa->add_transition(prefixed_f, '\0', "qf");
    new_ndfa->add_transition(prefixed_f, '\0', prefixed_inner_init);
  }

  delete ndfa_expr;
  return new_ndfa;
}

bool Star::_atomic(void) const { return false; }

string Star::to_string(void) const {
  return (expr->_atomic()) ? expr->to_string() + "*"
                           : "(" + expr->to_string() + ")*";
}

Plus::Plus(shared_ptr<Regex> e) : expr(e) {};

NDFA *Plus::to_ndfa(void) const {
  NDFA *ndfa_expr = expr->to_ndfa();
  if (!ndfa_expr)
    return nullptr;

  auto init_opt = ndfa_expr->get_inital_state();
  if (!init_opt) {
    delete ndfa_expr;
    return nullptr;
  }

  NDFA *new_ndfa = new NDFA();
  new_ndfa->add_state("q0", false);
  new_ndfa->add_state("qf", true);
  new_ndfa->mark_initial_state("q0");

  cpy_states(ndfa_expr, new_ndfa, "EXPR_");
  cpy_transitions(ndfa_expr, new_ndfa, "EXPR_");

  std::string prefixed_inner_init = "EXPR_" + init_opt.value();

  new_ndfa->add_transition("q0", '\0', prefixed_inner_init);

  for (const auto &final_state : ndfa_expr->get_final_states()) {
    std::string prefixed_f = "EXPR_" + final_state;
    new_ndfa->add_transition(prefixed_f, '\0', "qf");
    new_ndfa->add_transition(prefixed_f, '\0', prefixed_inner_init);
  }

  delete ndfa_expr;
  return new_ndfa;
}

bool Plus::_atomic(void) const { return false; }

string Plus::to_string(void) const {
  return (expr->_atomic()) ? expr->to_string() + "+"
                           : "(" + expr->to_string() + ")+";
}
} // namespace fa::regex
