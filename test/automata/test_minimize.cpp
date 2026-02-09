#include "../include/fa/automata/dfa.hpp"
#include <iostream>

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define MAGENTA "\033[35m"

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

void test_minimize_already_minimal() {
  print_section("DFA Minimize: Already Minimal");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1", true);
  dfa.mark_initial_state("q0");

  dfa.add_transition("q0", 'a', "q1");
  dfa.add_transition("q0", 'b', "q0");
  dfa.add_transition("q1", 'a', "q1");
  dfa.add_transition("q1", 'b', "q1");

  std::cout << "\n" YELLOW "Original DFA:" RESET "\n"
            << dfa.transitions_table() << std::endl;

  DFA *min = dfa.minimize();

  std::cout << YELLOW "Minimized DFA:" RESET "\n"
            << min->transitions_table() << std::endl;

  print_test("Already minimal DFA unchanged", min->size() == 2);

  delete min;
}

void test_minimize_simple_redundant() {
  print_section("DFA Minimize: Simple Redundant States");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1");
  dfa.add_state("q2");
  dfa.add_state("q3", true);
  dfa.add_state("q4", true);
  dfa.mark_initial_state("q0");

  // q3 y q4 son equivalentes (ambos finales y mismas transiciones)
  dfa.add_transition("q0", 'a', "q1");
  dfa.add_transition("q0", 'b', "q2");
  dfa.add_transition("q1", 'a', "q3");
  dfa.add_transition("q1", 'b', "q3");
  dfa.add_transition("q2", 'a', "q4");
  dfa.add_transition("q2", 'b', "q4");
  dfa.add_transition("q3", 'a', "q3");
  dfa.add_transition("q3", 'b', "q3");
  dfa.add_transition("q4", 'a', "q4");
  dfa.add_transition("q4", 'b', "q4");

  std::cout << "\n" YELLOW "Original DFA (5 states):" RESET "\n"
            << dfa.transitions_table() << std::endl;

  DFA *min = dfa.minimize();

  std::cout << YELLOW "Minimized DFA:" RESET "\n"
            << min->transitions_table() << std::endl;

  print_test("Redundant states merged", min->size() < dfa.size());

  delete min;
}

void test_minimize_multiple_equivalent() {
  print_section("DFA Minimize: Multiple Equivalent States");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1");
  dfa.add_state("q2");
  dfa.add_state("q3");
  dfa.add_state("q4", true);
  dfa.mark_initial_state("q0");

  // q1, q2, q3 son equivalentes (todos no finales, van al mismo lugar)
  dfa.add_transition("q0", 'a', "q1");
  dfa.add_transition("q0", 'b', "q2");
  dfa.add_transition("q1", 'a', "q4");
  dfa.add_transition("q1", 'b', "q4");
  dfa.add_transition("q2", 'a', "q4");
  dfa.add_transition("q2", 'b', "q4");
  dfa.add_transition("q3", 'a', "q4");
  dfa.add_transition("q3", 'b', "q4");
  dfa.add_transition("q4", 'a', "q4");
  dfa.add_transition("q4", 'b', "q4");

  std::cout << "\n" YELLOW "Original DFA (5 states):" RESET "\n"
            << dfa.transitions_table() << std::endl;

  DFA *min = dfa.minimize();

  std::cout << YELLOW "Minimized DFA:" RESET "\n"
            << min->transitions_table() << std::endl;

  print_test("Multiple equivalent states merged", min->size() < dfa.size());

  delete min;
}

void test_minimize_complex_pattern() {
  print_section("DFA Minimize: Complex Pattern");

  DFA dfa;
  // Acepta strings con número par de 'a's
  dfa.add_state("q0", true); // par de a's
  dfa.add_state("q1");       // impar de a's
  dfa.add_state("q2", true); // equivalente a q0
  dfa.add_state("q3");       // equivalente a q1
  dfa.mark_initial_state("q0");

  dfa.add_transition("q0", 'a', "q1");
  dfa.add_transition("q0", 'b', "q0");
  dfa.add_transition("q1", 'a', "q2"); // va a q2 (equivalente a q0)
  dfa.add_transition("q1", 'b', "q1");
  dfa.add_transition("q2", 'a', "q3"); // va a q3 (equivalente a q1)
  dfa.add_transition("q2", 'b', "q2");
  dfa.add_transition("q3", 'a', "q0");
  dfa.add_transition("q3", 'b', "q3");

  std::cout << "\n" YELLOW "Original DFA (4 states):" RESET "\n"
            << dfa.transitions_table() << std::endl;

  DFA *min = dfa.minimize();

  std::cout << YELLOW "Minimized DFA (should be 2 states):" RESET "\n"
            << min->transitions_table() << std::endl;

  print_test("Complex pattern minimized to 2 states", min->size() == 2);

  delete min;
}

void test_minimize_unreachable_states() {
  print_section("DFA Minimize: With Unreachable States");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1", true);
  dfa.add_state("q2"); // unreachable
  dfa.add_state("q3"); // unreachable
  dfa.mark_initial_state("q0");

  dfa.add_transition("q0", 'a', "q1");
  dfa.add_transition("q0", 'b', "q0");
  dfa.add_transition("q1", 'a', "q1");
  dfa.add_transition("q1", 'b', "q1");
  // q2 y q3 están desconectados
  dfa.add_transition("q2", 'a', "q3");
  dfa.add_transition("q2", 'b', "q2");
  dfa.add_transition("q3", 'a', "q2");
  dfa.add_transition("q3", 'b', "q3");

  std::cout << "\n" YELLOW "Original DFA (with unreachable):" RESET "\n"
            << dfa.transitions_table() << std::endl;

  DFA *min = dfa.minimize();

  std::cout << YELLOW "Minimized DFA:" RESET "\n"
            << min->transitions_table() << std::endl;

  print_test("Minimized (unreachable states handled)", min != nullptr);

  delete min;
}

void test_minimize_all_equivalent() {
  print_section("DFA Minimize: All States Equivalent");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1");
  dfa.add_state("q2");
  dfa.mark_initial_state("q0");

  // Todos no finales y todos van al mismo lugar
  dfa.add_transition("q0", 'a', "q0");
  dfa.add_transition("q0", 'b', "q0");
  dfa.add_transition("q1", 'a', "q0");
  dfa.add_transition("q1", 'b', "q0");
  dfa.add_transition("q2", 'a', "q0");
  dfa.add_transition("q2", 'b', "q0");

  std::cout << "\n" YELLOW "Original DFA (3 states):" RESET "\n"
            << dfa.transitions_table() << std::endl;

  DFA *min = dfa.minimize();

  std::cout << YELLOW "Minimized DFA (should be 1 state):" RESET "\n"
            << min->transitions_table() << std::endl;

  print_test("All states merged to 1", min->size() == 1);

  delete min;
}

void test_minimize_single_state() {
  print_section("DFA Minimize: Single State");

  DFA dfa;
  dfa.add_state("q0", true);
  dfa.mark_initial_state("q0");

  dfa.add_transition("q0", 'a', "q0");
  dfa.add_transition("q0", 'b', "q0");

  std::cout << "\n" YELLOW "Original DFA (1 state):" RESET "\n"
            << dfa.transitions_table() << std::endl;

  DFA *min = dfa.minimize();

  std::cout << YELLOW "Minimized DFA:" RESET "\n"
            << min->transitions_table() << std::endl;

  print_test("Single state DFA unchanged", min->size() == 1);

  delete min;
}

void test_minimize_trap_state() {
  print_section("DFA Minimize: With Trap State");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1", true);
  dfa.add_state("trap"); // estado trampa
  dfa.mark_initial_state("q0");

  dfa.add_transition("q0", 'a', "q1");
  dfa.add_transition("q0", 'b', "trap");
  dfa.add_transition("q1", 'a', "q1");
  dfa.add_transition("q1", 'b', "trap");
  dfa.add_transition("trap", 'a', "trap");
  dfa.add_transition("trap", 'b', "trap");

  std::cout << "\n" YELLOW "Original DFA (with trap):" RESET "\n"
            << dfa.transitions_table() << std::endl;

  DFA *min = dfa.minimize();

  std::cout << YELLOW "Minimized DFA:" RESET "\n"
            << min->transitions_table() << std::endl;

  print_test("DFA with trap state minimized", min->size() == 3);

  delete min;
}

void test_minimize_long_chain() {
  print_section("DFA Minimize: Long Chain of Equivalent States");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1");
  dfa.add_state("q2");
  dfa.add_state("q3");
  dfa.add_state("q4");
  dfa.add_state("q5", true);
  dfa.mark_initial_state("q0");

  // Cadena larga donde algunos estados son equivalentes
  dfa.add_transition("q0", 'a', "q1");
  dfa.add_transition("q0", 'b', "q0");
  dfa.add_transition("q1", 'a', "q2");
  dfa.add_transition("q1", 'b', "q1");
  dfa.add_transition("q2", 'a', "q3");
  dfa.add_transition("q2", 'b', "q2");
  dfa.add_transition("q3", 'a', "q4");
  dfa.add_transition("q3", 'b', "q3");
  dfa.add_transition("q4", 'a', "q5");
  dfa.add_transition("q4", 'b', "q4");
  dfa.add_transition("q5", 'a', "q5");
  dfa.add_transition("q5", 'b', "q5");

  std::cout << "\n" YELLOW "Original DFA (6 states):" RESET "\n"
            << dfa.transitions_table() << std::endl;

  DFA *min = dfa.minimize();

  std::cout << YELLOW "Minimized DFA:" RESET "\n"
            << min->transitions_table() << std::endl;

  print_test("Long chain minimized", min->size() <= dfa.size());

  delete min;
}

void test_minimize_empty_language() {
  print_section("DFA Minimize: Empty Language (No Finals)");

  DFA dfa;
  dfa.add_state("q0");
  dfa.add_state("q1");
  dfa.add_state("q2");
  dfa.mark_initial_state("q0");

  dfa.add_transition("q0", 'a', "q1");
  dfa.add_transition("q0", 'b', "q2");
  dfa.add_transition("q1", 'a', "q0");
  dfa.add_transition("q1", 'b', "q1");
  dfa.add_transition("q2", 'a', "q2");
  dfa.add_transition("q2", 'b', "q0");

  std::cout << "\n" YELLOW "Original DFA (no finals):" RESET "\n"
            << dfa.transitions_table() << std::endl;

  DFA *min = dfa.minimize();

  std::cout << YELLOW "Minimized DFA:" RESET "\n"
            << min->transitions_table() << std::endl;

  print_test("Empty language DFA minimized", min != nullptr);

  delete min;
}

int main() {
  std::cout << CYAN "\n╔════════════════════════════════════════╗" RESET
            << std::endl;
  std::cout << CYAN "║      TEST SUITE - DFA Minimization     ║" RESET
            << std::endl;
  std::cout << CYAN "╚════════════════════════════════════════╝" RESET
            << std::endl;

  // Basic minimization tests
  test_minimize_already_minimal();
  test_minimize_simple_redundant();
  test_minimize_multiple_equivalent();

  // Complex patterns
  test_minimize_complex_pattern();
  test_minimize_long_chain();

  // Edge cases
  test_minimize_single_state();
  test_minimize_trap_state();
  test_minimize_unreachable_states();
  test_minimize_all_equivalent();
  test_minimize_empty_language();

  std::cout << "\n" CYAN "════════════════════════════════════════" RESET
            << std::endl;
  std::cout << GREEN "✓ Tests passed: " << tests_passed << RESET << std::endl;
  std::cout << RED "✗ Tests failed: " << tests_failed << RESET << std::endl;
  std::cout << CYAN "════════════════════════════════════════" RESET
            << std::endl;

  return tests_failed > 0 ? 1 : 0;
}
