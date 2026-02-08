#include "../../include/regex.hpp"
#include "../../include/dfa.hpp"
#include "../../include/fa.hpp"
#include "../../include/ndfa.hpp"
#include <cstddef>
#include <format>
#include <stdexcept>
using namespace std;

Regex::~Regex() {
  if (_dfa_cache != nullptr) {
    delete _dfa_cache;
  }
}

DFA *Regex::dfa() const {
  if (_dfa_cache == nullptr) {
    NDFA *ndfa = to_ndfa();
    DFA *dfa = ndfa->determinize();
    _dfa_cache = dfa->minimize();
    delete ndfa;
    delete dfa;
  }
  return _dfa_cache;
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
  return res;
}

bool Empty::_atomic(void) const { return true; }

string Empty::to_string(void) const { return "∅"; }

/*LAMBDA*/

NDFA *Lambda::to_ndfa(void) const {
  NDFA *res = new NDFA();
  res->add_state("q0", true);

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

static void cpy_states_concat(NDFA *source, string prefix) {}

NDFA *Concat::to_ndfa(void) const {

  NDFA *ndfa_expr1 = expr1->to_ndfa();
  NDFA *ndfa_expr2 = expr2->to_ndfa();

  NDFA *new_ndfa = new NDFA();

  delete ndfa_expr1;
  delete ndfa_expr2;
  return new_ndfa;
}

bool Concat::_atomic(void) const { return true; }

string Concat::to_string(void) const {}
