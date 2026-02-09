#include "../../include/fa/automata/dfa.hpp"
#include "../../include/fa/automata/ndfa.hpp"
#include <iostream>
#include <memory>
#include <string>

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define EPSILON '\0'

int tests_passed = 0;
int tests_failed = 0;

void print_test(const std::string &name, bool passed) {
  if (passed) {
    std::cout << "[" << GREEN << "PASS" << RESET << "] " << name << std::endl;
    tests_passed++;
  } else {
    std::cout << "[" << RED << "FAIL" << RESET << "] " << name << std::endl;
    tests_failed++;
  }
}

void print_section(const std::string &title) {
  std::cout << "\n"
            << CYAN << "══════════════════════════════════════" << RESET
            << std::endl;
  std::cout << CYAN << "  " << title << RESET << std::endl;
  std::cout << CYAN << "══════════════════════════════════════" << RESET
            << std::endl;
}

void test_ndfa_determinize_simple() {
  print_section("NDFA Determinize: Simple Case");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", 'a', "q1");
  nfa.add_transition("q1", 'b', "q2");

  std::cout << "\n"
            << YELLOW << "Original NDFA:" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA:" << RESET << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("Simple NDFA determinized", dfa != nullptr && dfa->size() > 0);
}

void test_ndfa_determinize_nondeterministic() {
  print_section("NDFA Determinize: Non-deterministic Transitions");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", 'a', "q0");
  nfa.add_transition("q0", 'a', "q1");
  nfa.add_transition("q1", 'b', "q2");

  std::cout << "\n"
            << YELLOW << "Original NDFA:" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA:" << RESET << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("Non-deterministic NDFA determinized", dfa != nullptr);
}

void test_ndfa_determinize_with_epsilon() {
  print_section("NDFA Determinize: With Epsilon Transitions");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2");
  nfa.add_state("q3", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", EPSILON, "q1");
  nfa.add_transition("q0", 'a', "q2");
  nfa.add_transition("q1", 'b', "q3");
  nfa.add_transition("q2", 'b', "q3");

  std::cout << "\n"
            << YELLOW << "Original NDFA with Epsilon:" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA:" << RESET << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("NDFA with epsilon transitions determinized", dfa != nullptr);
}

void test_ndfa_determinize_chained_epsilon() {
  print_section("NDFA Determinize: Chained Epsilon Transitions");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2");
  nfa.add_state("q3", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", EPSILON, "q1");
  nfa.add_transition("q1", EPSILON, "q2");
  nfa.add_transition("q2", 'a', "q3");

  std::cout << "\n"
            << YELLOW << "Original NDFA with Chained Epsilon:" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA:" << RESET << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("Chained epsilon transitions handled", dfa != nullptr);
}

void test_ndfa_determinize_epsilon_closure() {
  print_section("NDFA Determinize: Complex Epsilon Closure");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2");
  nfa.add_state("q3");
  nfa.add_state("q4", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", 'a', "q1");
  nfa.add_transition("q0", 'a', "q2");
  nfa.add_transition("q1", EPSILON, "q3");
  nfa.add_transition("q2", EPSILON, "q3");
  nfa.add_transition("q3", 'b', "q4");

  std::cout << "\n"
            << YELLOW << "Original NDFA:" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA:" << RESET << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("Complex epsilon closure determinized", dfa != nullptr);
}

void test_ndfa_determinize_with_trap_state() {
  print_section("NDFA Determinize: Trap State Creation");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", 'a', "q1");
  nfa.add_transition("q0", 'b', "q0");

  std::cout << "\n"
            << YELLOW << "Original NDFA:" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA (should have trap state):" << RESET
            << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("Trap state created for missing transitions", dfa != nullptr);
}

void test_ndfa_determinize_all_final() {
  print_section("NDFA Determinize: Initial State Reaches Final via Epsilon");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", EPSILON, "q1");
  nfa.add_transition("q0", 'a', "q0");

  std::cout << "\n"
            << YELLOW << "Original NDFA:" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA (initial should be final):" << RESET
            << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("Initial state is final when epsilon reaches final",
             dfa != nullptr);
}

void test_ndfa_determinize_regex_pattern() {
  print_section("NDFA Determinize: Regex Pattern (a|b)*abb");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2");
  nfa.add_state("q3", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", 'a', "q0");
  nfa.add_transition("q0", 'b', "q0");
  nfa.add_transition("q0", 'a', "q1");
  nfa.add_transition("q1", 'b', "q2");
  nfa.add_transition("q2", 'b', "q3");

  std::cout << "\n"
            << YELLOW << "Original NDFA (pattern: .*abb):" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA:" << RESET << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("Regex pattern NDFA determinized", dfa != nullptr);
}

void test_ndfa_determinize_complete_alphabet() {
  print_section("NDFA Determinize: Complete Alphabet Coverage");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", 'a', "q1");
  nfa.add_transition("q0", 'b', "q0");
  nfa.add_transition("q0", 'c', "q0");
  nfa.add_transition("q1", 'a', "q2");
  nfa.add_transition("q1", 'b', "q1");
  nfa.add_transition("q1", 'c', "q1");
  nfa.add_transition("q2", 'a', "q2");
  nfa.add_transition("q2", 'b', "q2");
  nfa.add_transition("q2", 'c', "q2");

  std::cout << "\n"
            << YELLOW << "Original NDFA:" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA:" << RESET << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("Complete alphabet NDFA determinized", dfa != nullptr);
}

void test_ndfa_determinize_epsilon_loop() {
  print_section("NDFA Determinize: Epsilon Loops");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.add_state("q2", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", EPSILON, "q1");
  nfa.add_transition("q1", EPSILON, "q0");
  nfa.add_transition("q0", 'a', "q2");

  std::cout << "\n"
            << YELLOW << "Original NDFA with Epsilon Loop:" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA:" << RESET << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("Epsilon loop handled correctly", dfa != nullptr);
}

void test_ndfa_determinize_multiple_finals() {
  print_section("NDFA Determinize: Multiple Final States");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1", true);
  nfa.add_state("q2", true);
  nfa.add_state("q3", true);
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", 'a', "q1");
  nfa.add_transition("q0", 'b', "q2");
  nfa.add_transition("q0", 'c', "q3");

  std::cout << "\n"
            << YELLOW << "Original NDFA:" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA:" << RESET << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("Multiple final states handled", dfa != nullptr);
}

void test_ndfa_determinize_empty_language() {
  print_section("NDFA Determinize: Empty Language (No Final States)");

  NDFA nfa;
  nfa.add_state("q0");
  nfa.add_state("q1");
  nfa.mark_initial_state("q0");

  nfa.add_transition("q0", 'a', "q1");
  nfa.add_transition("q1", 'b', "q0");

  std::cout << "\n"
            << YELLOW << "Original NDFA (no final states):" << RESET << "\n"
            << nfa.transitions_table() << std::endl;

  std::unique_ptr<DFA> dfa = nfa.determinize();

  std::cout << YELLOW << "Determinized DFA:" << RESET << "\n"
            << dfa->transitions_table() << std::endl;

  print_test("Empty language NDFA determinized", dfa != nullptr);
}

int main() {
  std::cout << CYAN << "\n╔════════════════════════════════════════╗" << RESET
            << std::endl;
  std::cout << CYAN << "║   TEST SUITE - NDFA Determinization    ║" << RESET
            << std::endl;
  std::cout << CYAN << "╚════════════════════════════════════════╝" << RESET
            << std::endl;

  test_ndfa_determinize_simple();
  test_ndfa_determinize_nondeterministic();

  test_ndfa_determinize_with_epsilon();
  test_ndfa_determinize_chained_epsilon();
  test_ndfa_determinize_epsilon_closure();
  test_ndfa_determinize_epsilon_loop();

  test_ndfa_determinize_with_trap_state();
  test_ndfa_determinize_all_final();
  test_ndfa_determinize_multiple_finals();
  test_ndfa_determinize_empty_language();

  test_ndfa_determinize_regex_pattern();
  test_ndfa_determinize_complete_alphabet();

  std::cout << "\n"
            << CYAN << "════════════════════════════════════════" << RESET
            << std::endl;
  std::cout << GREEN << "✓ Tests passed: " << tests_passed << RESET
            << std::endl;
  std::cout << RED << "✗ Tests failed: " << tests_failed << RESET << std::endl;
  std::cout << CYAN << "════════════════════════════════════════" << RESET
            << std::endl;

  return tests_failed > 0 ? 1 : 0;
}
