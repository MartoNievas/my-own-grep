#include "../include/dfa.hpp"
#include "../include/ndfa.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"

int tests_passed = 0;
int tests_failed = 0;

void print_test(const std::string &name, bool passed) {
  if (passed) {
    std::cout << "[" GREEN "PASS" RESET "] " << name << std::endl;
    tests_passed++;
  } else {
    std::cout << "[" RED "FAIL" RESET "] " << name << std::endl;
    tests_failed++;
  }
}

void print_section(const std::string &title) {
  std::cout << "\n" CYAN "══════════════════════════════════════" RESET
            << std::endl;
  std::cout << CYAN "  " << title << RESET << std::endl;
  std::cout << CYAN "══════════════════════════════════════" RESET << std::endl;
}

void test_dfa_add_state() {
  print_section("DFA: Add States");

  DFA dfa;

  try {
    dfa.add_state("q0");
    print_test("Add simple state", dfa.size() == 1);
  } catch (...) {
    print_test("Add simple state", false);
  }

  dfa.add_state("q1");
  dfa.add_state("q2");
  print_test("Add multiple states", dfa.size() == 3);

  dfa.add_state("q3", true);
  print_test("Add final state", dfa.size() == 4);

  bool exception_thrown = false;
  try {
    dfa.add_state("q0");
  } catch (const std::invalid_argument &) {
    exception_thrown = true;
  }
  print_test("Reject duplicated state", exception_thrown);

  std::cout << "\nDFA Table:\n" << dfa.transitions_table() << std::endl;
}

void test_dfa_mark_initial_state() {
  print_section("DFA: Mark Initial State");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1");
  dfa.add_state("q2");

  try {
    dfa.mark_initial_state("q0");
    print_test("Mark valid initial state", true);
  } catch (...) {
    print_test("Mark valid initial state", false);
  }

  bool exception_thrown = false;
  try {
    dfa.mark_initial_state("q99");
  } catch (const std::invalid_argument &) {
    exception_thrown = true;
  }
  print_test("Reject non-existing initial state", exception_thrown);

  dfa.mark_initial_state("q1");
  print_test("Change initial state", true);

  std::cout << "\nDFA Table:\n" << dfa.transitions_table() << std::endl;
}

void test_dfa_add_transitions() {
  print_section("DFA: Add Transitions");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1");
  dfa.add_state("q2", true);
  dfa.mark_initial_state("q0");

  dfa.add_transition("q0", 'a', "q1");
  print_test("Add simple transition", true);

  dfa.add_transition("q1", 'b', "q2");
  dfa.add_transition("q2", 'a', "q0");
  print_test("Add multiple transitions", true);

  dfa.add_transition("q0", 'b', "q0");
  dfa.add_transition("q1", 'a', "q1");
  print_test("Add loops", true);

  dfa.add_transition("q0", 'c', "q99");
  print_test("Transition to non-existing state (ignored)", dfa.size() == 3);

  std::cout << "\nDFA Table:\n" << dfa.transitions_table() << std::endl;
}

void test_dfa_normalize() {
  print_section("DFA: Normalize States");

  DFA dfa;
  dfa.add_state("start");
  dfa.add_state("middle");
  dfa.add_state("end", true);
  dfa.add_state("other");
  dfa.mark_initial_state("start");

  dfa.add_transition("start", 'a', "middle");
  dfa.add_transition("middle", 'b', "end");
  dfa.add_transition("end", 'c', "other");

  std::cout << "\nBefore normalization:\n"
            << dfa.transitions_table() << std::endl;

  dfa.normalize_states();

  std::cout << "\nAfter normalization:\n"
            << dfa.transitions_table() << std::endl;

  print_test("States normalized", dfa.size() == 4);
}

void test_dfa_complex_construction() {
  print_section("DFA: Complex Construction");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1");
  dfa.add_state("q2");
  dfa.add_state("q3", true);
  dfa.add_state("q4", true);
  dfa.mark_initial_state("q0");

  dfa.add_transition("q0", 'a', "q1");
  dfa.add_transition("q0", 'b', "q2");
  dfa.add_transition("q1", 'a', "q3");
  dfa.add_transition("q1", 'b', "q4");
  dfa.add_transition("q2", 'a', "q4");
  dfa.add_transition("q2", 'b', "q3");
  dfa.add_transition("q3", 'a', "q3");
  dfa.add_transition("q3", 'b', "q3");
  dfa.add_transition("q4", 'a', "q4");
  dfa.add_transition("q4", 'b', "q4");

  print_test("Complex DFA constructed", dfa.size() == 5);

  std::cout << "\nComplex DFA Table:\n" << dfa.transitions_table() << std::endl;
}

void test_ndfa_add_state() {
  print_section("NDFA: Add States");

  NDFA nfa;

  try {
    nfa.add_state("q0");
    print_test("Add simple state", nfa.size() == 1);
  } catch (...) {
    print_test("Add simple state", false);
  }

  nfa.add_state("q1");
  nfa.add_state("q2");
  print_test("Add multiple states", nfa.size() == 3);

  nfa.add_state("q3", true);
  print_test("Add final state", nfa.size() == 4);

  bool exception_thrown = false;
  try {
    nfa.add_state("q0");
  } catch (const std::invalid_argument &) {
    exception_thrown = true;
  }
  print_test("Reject duplicated state", exception_thrown);

  std::cout << "\nNDFA Table:\n" << nfa.transitions_table() << std::endl;
}

void test_ndfa_mark_initial_state() {
  print_section("NDFA: Mark Initial State");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2");

  try {
    nfa.mark_initial_state("q0");
    print_test("Mark valid initial state", true);
  } catch (...) {
    print_test("Mark valid initial state", false);
  }

  bool exception_thrown = false;
  try {
    nfa.mark_initial_state("q99");
  } catch (const std::invalid_argument &) {
    exception_thrown = true;
  }
  print_test("Reject non-existing initial state", exception_thrown);

  std::cout << "\nNDFA Table:\n" << nfa.transitions_table() << std::endl;
}

void test_ndfa_add_transitions() {
  print_section("NDFA: Add Transitions");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", 'a', "q1");
  print_test("Add simple transition", true);

  nfa.add_transition("q0", 'a', "q0");
  print_test("Add second transition with same symbol (non-deterministic)",
             true);

  nfa.add_transition("q1", 'b', "q2");
  nfa.add_transition("q0", 'b', "q2");
  print_test("Add multiple transitions", true);

  std::cout << "\nNDFA Table:\n" << nfa.transitions_table() << std::endl;
}

void test_ndfa_epsilon_transitions() {
  print_section("NDFA: Epsilon Transitions");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2");
  nfa.add_state("q3", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", EPSILON, "q1");
  print_test("Add epsilon transition", true);

  nfa.add_transition("q1", 'a', "q2");
  nfa.add_transition("q2", EPSILON, "q3");
  print_test("Combine epsilon with normal transitions", true);

  nfa.add_transition("q0", EPSILON, "q2");
  print_test("Multiple epsilon transitions from same state", true);

  std::cout << "\nNDFA with Epsilon Table:\n"
            << nfa.transitions_table() << std::endl;
}

void test_ndfa_normalize() {
  print_section("NDFA: Normalize States");

  NDFA nfa;
  nfa.add_state("start");
  nfa.add_state("process");
  nfa.add_state("end", true);
  nfa.mark_initial_state("start");

  nfa.add_transition("start", 'a', "process");
  nfa.add_transition("start", 'a', "end");
  nfa.add_transition("process", 'b', "end");
  nfa.add_transition("process", EPSILON, "end");

  std::cout << "\nBefore normalization:\n"
            << nfa.transitions_table() << std::endl;

  nfa.normalize_states();

  std::cout << "\nAfter normalization:\n"
            << nfa.transitions_table() << std::endl;

  print_test("States normalized", nfa.size() == 3);
}

void test_ndfa_complex_construction() {
  print_section("NDFA: Complex Construction");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2");
  nfa.add_state("q3");
  nfa.add_state("q4", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", 'a', "q0");
  nfa.add_transition("q0", 'a', "q1");
  nfa.add_transition("q0", 'b', "q0");
  nfa.add_transition("q1", 'b', "q2");
  nfa.add_transition("q2", EPSILON, "q3");
  nfa.add_transition("q3", 'c', "q4");
  nfa.add_transition("q0", EPSILON, "q2");

  print_test("Complex NDFA constructed", nfa.size() == 5);

  std::cout << "\nComplex NDFA Table:\n"
            << nfa.transitions_table() << std::endl;
}

void test_ndfa_multiple_epsilon() {
  print_section("NDFA: Chained Epsilon Transitions");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2");
  nfa.add_state("q3");
  nfa.add_state("q4", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", EPSILON, "q1");
  nfa.add_transition("q1", EPSILON, "q2");
  nfa.add_transition("q2", EPSILON, "q3");
  nfa.add_transition("q3", 'a', "q4");

  print_test("Chained epsilon transitions", true);

  std::cout << "\nChained Epsilon NDFA Table:\n"
            << nfa.transitions_table() << std::endl;
}

void test_mixed_alphabet() {
  print_section("Mixed Alphabets");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1", true);
  dfa.mark_initial_state("q0");

  dfa.add_transition("q0", 'a', "q1");
  dfa.add_transition("q0", 'b', "q1");
  dfa.add_transition("q0", '0', "q1");
  dfa.add_transition("q0", '1', "q1");
  dfa.add_transition("q1", 'a', "q0");

  print_test("DFA with letters and digits", true);

  std::cout << "\nMixed Alphabet DFA Table:\n"
            << dfa.transitions_table() << std::endl;

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", 'x', "q1");
  nfa.add_transition("q0", 'y', "q1");
  nfa.add_transition("q0", '+', "q1");
  nfa.add_transition("q0", '-', "q1");

  print_test("NDFA with special symbols", true);

  std::cout << "\nSpecial Symbols NDFA Table:\n"
            << nfa.transitions_table() << std::endl;
}

void test_edge_cases() {
  print_section("Edge Cases");

  DFA dfa1;
  dfa1.add_state("q0", true);
  dfa1.mark_initial_state("q0");
  print_test("DFA with single state (initial and final)", dfa1.size() == 1);

  DFA dfa2;
  dfa2.add_state("q0");
  dfa2.mark_initial_state("q0");
  print_test("DFA with only initial state (no finals)", dfa2.size() == 1);

  NDFA nfa1;
  nfa1.add_state("q0", true);
  nfa1.mark_initial_state("q0");
  nfa1.add_transition("q0", EPSILON, "q0");
  print_test("NDFA with epsilon loop to itself", nfa1.size() == 1);

  NDFA nfa2;
  nfa2.add_state("q0");
  nfa2.add_state("q1");
  nfa2.add_state("q2", true);
  nfa2.mark_initial_state("q0");
  nfa2.add_transition("q0", EPSILON, "q1");
  nfa2.add_transition("q1", EPSILON, "q2");
  nfa2.add_transition("q2", EPSILON, "q0");
  print_test("NDFA with epsilon cycle", nfa2.size() == 3);

  std::cout << "\nEpsilon Cycle NDFA Table:\n"
            << nfa2.transitions_table() << std::endl;
}

int main() {
  std::cout << CYAN "\n╔════════════════════════════════════════╗" RESET
            << std::endl;
  std::cout << CYAN "║     TEST SUITE - Automata Construction  ║" RESET
            << std::endl;
  std::cout << CYAN "╚════════════════════════════════════════╝" RESET
            << std::endl;

  test_dfa_add_state();
  test_dfa_mark_initial_state();
  test_dfa_add_transitions();
  test_dfa_normalize();
  test_dfa_complex_construction();

  test_ndfa_add_state();
  test_ndfa_mark_initial_state();
  test_ndfa_add_transitions();
  test_ndfa_epsilon_transitions();
  test_ndfa_normalize();
  test_ndfa_complex_construction();
  test_ndfa_multiple_epsilon();

  test_mixed_alphabet();
  test_edge_cases();

  std::cout << "\n" CYAN "════════════════════════════════════════" RESET
            << std::endl;
  std::cout << GREEN "✓ Tests passed: " << tests_passed << RESET << std::endl;
  std::cout << RED "✗ Tests failed: " << tests_failed << RESET << std::endl;
  std::cout << CYAN "════════════════════════════════════════" RESET
            << std::endl;

  return tests_failed > 0 ? 1 : 0;
}
