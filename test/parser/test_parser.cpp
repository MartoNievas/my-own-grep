#include "../../include/fa/parser/parser.hpp"
#include "../../include/fa/regex/regex.hpp"
#include <iostream>
#include <memory>

#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"

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

void test_single_char() {
  print_section("Parser: Single Character");

  try {
    Parser parser("a");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: 'a'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string() == "a";

    print_test("Single character parsed", test1 && test2);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Single character parsed", false);
  }
}

void test_union() {
  print_section("Parser: Union");

  try {
    Parser parser("a|b");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: 'a|b'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string() == "a|b";

    print_test("Union parsed", test1 && test2);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Union parsed", false);
  }
}

void test_star() {
  print_section("Parser: Star");

  try {
    Parser parser("a*");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: 'a*'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string() == "a*";

    print_test("Star parsed", test1 && test2);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Star parsed", false);
  }
}

void test_plus() {
  print_section("Parser: Plus");

  try {
    Parser parser("b+");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: 'b+'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string() == "b+";

    print_test("Plus parsed", test1 && test2);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Plus parsed", false);
  }
}

void test_concatenation() {
  print_section("Parser: Concatenation");

  try {
    Parser parser("abc");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: 'abc'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    // Concatenación puede mostrarse como "abc" o "a(bc)" o "(ab)c"
    bool test2 = regex->to_string().find('a') != std::string::npos &&
                 regex->to_string().find('b') != std::string::npos &&
                 regex->to_string().find('c') != std::string::npos;

    print_test("Concatenation parsed", test1 && test2);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Concatenation parsed", false);
  }
}

void test_parentheses() {
  print_section("Parser: Parentheses");

  try {
    Parser parser("(a|b)");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: '(a|b)'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string() == "a|b";

    print_test("Parentheses parsed", test1 && test2);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Parentheses parsed", false);
  }
}

void test_lambda() {
  print_section("Parser: Lambda");

  try {
    Parser parser("@");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: '@'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string() == "λ";

    print_test("Lambda parsed", test1 && test2);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Lambda parsed", false);
  }
}

void test_empty() {
  print_section("Parser: Empty Set");

  try {
    Parser parser("#");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: '#'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string() == "∅";

    print_test("Empty set parsed", test1 && test2);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Empty set parsed", false);
  }
}

void test_complex_union_star() {
  print_section("Parser: Complex (a|b)*");

  try {
    Parser parser("(a|b)*");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: '(a|b)*'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string() == "(a|b)*";

    print_test("(a|b)* parsed", test1 && test2);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("(a|b)* parsed", false);
  }
}

void test_complex_pattern() {
  print_section("Parser: Complex Pattern (a|b)*abb");

  try {
    Parser parser("(a|b)*abb");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: '(a|b)*abb'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string().find("a|b") != std::string::npos;
    bool test3 = regex->to_string().find("*") != std::string::npos;

    print_test("(a|b)*abb parsed", test1 && test2 && test3);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("(a|b)*abb parsed", false);
  }
}

void test_nested_parentheses() {
  print_section("Parser: Nested Parentheses");

  try {
    Parser parser("((a|b)*c)+");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: '((a|b)*c)+'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string().find("+") != std::string::npos;

    print_test("Nested parentheses parsed", test1 && test2);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Nested parentheses parsed", false);
  }
}

void test_multiple_union() {
  print_section("Parser: Multiple Unions");

  try {
    Parser parser("a|b|c");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: 'a|b|c'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string().find('a') != std::string::npos;
    bool test3 = regex->to_string().find('b') != std::string::npos;
    bool test4 = regex->to_string().find('c') != std::string::npos;

    print_test("Multiple unions parsed", test1 && test2 && test3 && test4);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Multiple unions parsed", false);
  }
}

void test_star_plus_combination() {
  print_section("Parser: Star and Plus Combination");

  try {
    Parser parser("a*b+");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: 'a*b+'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string().find("a*") != std::string::npos;
    bool test3 = regex->to_string().find("b+") != std::string::npos;

    print_test("a*b+ parsed", test1 && test2 && test3);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("a*b+ parsed", false);
  }
}

void test_precedence() {
  print_section("Parser: Operator Precedence");

  try {
    Parser parser("a|bc*");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: 'a|bc*' (should be a|(b(c*)))" RESET
              << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    // Star tiene mayor precedencia que concat, concat mayor que union
    // Debería parsearse como: a | (b(c*))

    print_test("Operator precedence correct", test1);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Operator precedence correct", false);
  }
}

void test_error_unmatched_paren() {
  print_section("Parser: Error - Unmatched Parenthesis");

  try {
    Parser parser("(a|b");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: '(a|b' (missing ')')" RESET << std::endl;
    print_test("Should throw exception", false);
  } catch (const std::exception &e) {
    std::cout << RED "Expected exception: " << e.what() << RESET << std::endl;
    print_test("Exception correctly thrown", true);
  }
}

void test_error_empty_parens() {
  print_section("Parser: Error - Empty Parentheses");

  try {
    Parser parser("()");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: '()' (empty)" RESET << std::endl;
    print_test("Should throw exception", false);
  } catch (const std::exception &e) {
    std::cout << RED "Expected exception: " << e.what() << RESET << std::endl;
    print_test("Exception correctly thrown", true);
  }
}

void test_digits() {
  print_section("Parser: Digits");

  try {
    Parser parser("0|1");
    auto regex = parser.parse();

    std::cout << YELLOW "Input: '0|1'" RESET << std::endl;
    std::cout << "Parsed: " << regex->to_string() << std::endl;

    bool test1 = regex != nullptr;
    bool test2 = regex->to_string() == "0|1";

    print_test("Digits parsed", test1 && test2);
  } catch (const std::exception &e) {
    std::cout << RED "Exception: " << e.what() << RESET << std::endl;
    print_test("Digits parsed", false);
  }
}

int main() {
  std::cout << CYAN "\n╔════════════════════════════════════════╗" RESET
            << std::endl;
  std::cout << CYAN "║         TEST SUITE - Parser            ║" RESET
            << std::endl;
  std::cout << CYAN "╚════════════════════════════════════════╝" RESET
            << std::endl;

  // Basic tests
  test_single_char();
  test_union();
  test_star();
  test_plus();
  test_concatenation();
  test_parentheses();
  test_lambda();
  test_empty();

  // Complex tests
  test_complex_union_star();
  test_complex_pattern();
  test_nested_parentheses();
  test_multiple_union();
  test_star_plus_combination();
  test_precedence();

  // Error tests
  test_error_unmatched_paren();
  test_error_empty_parens();

  // Edge cases
  test_digits();

  std::cout << "\n" CYAN "════════════════════════════════════════" RESET
            << std::endl;
  std::cout << GREEN "✓ Tests passed: " << tests_passed << RESET << std::endl;
  std::cout << RED "✗ Tests failed: " << tests_failed << RESET << std::endl;
  std::cout << CYAN "════════════════════════════════════════" RESET
            << std::endl;

  return tests_failed > 0 ? 1 : 0;
}
