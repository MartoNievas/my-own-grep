#include "../../include/fa/lexer/lexer.hpp"
#include "../../include/fa/lexer/token.hpp"
#include <iostream>
#include <vector>

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

void print_tokens(const std::vector<Token> &tokens) {
  for (const auto &token : tokens) {
    std::cout << "  " << token.to_string() << std::endl;
  }
}

void test_single_char() {
  print_section("Lexer: Single Character");

  Lexer lexer("a");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: 'a'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 2; // CHAR('a'), END
  bool test2 = tokens[0].type == TOKEN_TYPE::LITERAL && tokens[0].value == 'a';
  bool test3 = tokens[1].type == TOKEN_TYPE::END;

  print_test("Single char tokenized", test1 && test2 && test3);
}

void test_union_operator() {
  print_section("Lexer: Union Operator");

  Lexer lexer("a|b");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: 'a|b'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 4; // CHAR('a'), UNION, CHAR('b'), END
  bool test2 = tokens[0].type == TOKEN_TYPE::LITERAL;
  bool test3 = tokens[1].type == TOKEN_TYPE::UNION;
  bool test4 = tokens[2].type == TOKEN_TYPE::LITERAL;

  print_test("Union operator tokenized", test1 && test2 && test3 && test4);
}

void test_star_operator() {
  print_section("Lexer: Star Operator");

  Lexer lexer("a*");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: 'a*'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 3; // CHAR('a'), STAR, END
  bool test2 = tokens[0].type == TOKEN_TYPE::LITERAL;
  bool test3 = tokens[1].type == TOKEN_TYPE::STAR;

  print_test("Star operator tokenized", test1 && test2 && test3);
}

void test_plus_operator() {
  print_section("Lexer: Plus Operator");

  Lexer lexer("b+");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: 'b+'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 3; // CHAR('b'), PLUS, END
  bool test2 = tokens[0].type == TOKEN_TYPE::LITERAL;
  bool test3 = tokens[1].type == TOKEN_TYPE::PLUS;

  print_test("Plus operator tokenized", test1 && test2 && test3);
}

void test_parentheses() {
  print_section("Lexer: Parentheses");

  Lexer lexer("(a)");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: '(a)'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 4; // OPAREN, CHAR('a'), CPAREN, END
  bool test2 = tokens[0].type == TOKEN_TYPE::OPAREN;
  bool test3 = tokens[1].type == TOKEN_TYPE::LITERAL;
  bool test4 = tokens[2].type == TOKEN_TYPE::CPAREN;

  print_test("Parentheses tokenized", test1 && test2 && test3 && test4);
}

void test_lambda() {
  print_section("Lexer: Lambda (@)");

  Lexer lexer("@");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: '@'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 2; // LAMBDA, END
  bool test2 = tokens[0].type == TOKEN_TYPE::LAMBDA;

  print_test("Lambda tokenized", test1 && test2);
}

void test_empty() {
  print_section("Lexer: Empty Set (#)");

  Lexer lexer("#");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: '#'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 2; // EMPTY, END
  bool test2 = tokens[0].type == TOKEN_TYPE::EMPTY;

  print_test("Empty set tokenized", test1 && test2);
}

void test_simple_concat() {
  print_section("Lexer: Simple Concatenation");

  Lexer lexer("abc");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: 'abc'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 4; // CHAR('a'), CHAR('b'), CHAR('c'), END
  bool test2 = tokens[0].value == 'a';
  bool test3 = tokens[1].value == 'b';
  bool test4 = tokens[2].value == 'c';

  print_test("Concatenation tokenized", test1 && test2 && test3 && test4);
}

void test_complex_expression_1() {
  print_section("Lexer: Complex Expression (a|b)*");

  Lexer lexer("(a|b)*");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: '(a|b)*'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 7; // (, a, |, b, ), *, END
  bool test2 = tokens[0].type == TOKEN_TYPE::OPAREN;
  bool test3 = tokens[5].type == TOKEN_TYPE::STAR;

  print_test("Complex expression (a|b)* tokenized", test1 && test2 && test3);
}

void test_complex_expression_2() {
  print_section("Lexer: Complex Expression (a|b)*abb");

  Lexer lexer("(a|b)*abb");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: '(a|b)*abb'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 10; // (, a, |, b, ), *, a, b, b, END
  bool test2 = tokens[0].type == TOKEN_TYPE::OPAREN;
  bool test3 = tokens[5].type == TOKEN_TYPE::STAR;
  bool test4 = tokens[6].value == 'a';
  bool test5 = tokens[7].value == 'b';
  bool test6 = tokens[8].value == 'b';

  print_test("Complex expression (a|b)*abb tokenized",
             test1 && test2 && test3 && test4 && test5 && test6);
}

void test_with_lambda() {
  print_section("Lexer: Expression with Lambda");

  Lexer lexer("a@b");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: 'a@b'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 4; // CHAR('a'), LAMBDA, CHAR('b'), END
  bool test2 = tokens[0].type == TOKEN_TYPE::LITERAL;
  bool test3 = tokens[1].type == TOKEN_TYPE::LAMBDA;
  bool test4 = tokens[2].type == TOKEN_TYPE::LITERAL;

  print_test("Expression with lambda tokenized",
             test1 && test2 && test3 && test4);
}

void test_digits_and_special() {
  print_section("Lexer: Digits and Special Characters");

  Lexer lexer("0|1");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: '0|1'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 4; // CHAR('0'), UNION, CHAR('1'), END
  bool test2 = tokens[0].value == '0';
  bool test3 = tokens[2].value == '1';

  print_test("Digits tokenized", test1 && test2 && test3);
}

void test_whitespace_handling() {
  print_section("Lexer: Whitespace Handling");

  Lexer lexer("a | b");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: 'a | b' (with spaces)" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 4; // CHAR('a'), UNION, CHAR('b'), END
  bool test2 = tokens[0].type == TOKEN_TYPE::LITERAL;
  bool test3 = tokens[1].type == TOKEN_TYPE::UNION;

  print_test("Whitespace correctly skipped", test1 && test2 && test3);
}

void test_empty_input() {
  print_section("Lexer: Empty Input");

  Lexer lexer("");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: '' (empty)" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens.size() == 1; // Only END
  bool test2 = tokens[0].type == TOKEN_TYPE::END;

  print_test("Empty input handled", test1 && test2);
}

void test_nested_parentheses() {
  print_section("Lexer: Nested Parentheses");

  Lexer lexer("((a|b)*c)+");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: '((a|b)*c)+'" RESET << std::endl;
  print_tokens(tokens);

  bool test1 = tokens[0].type == TOKEN_TYPE::OPAREN;
  bool test2 = tokens[1].type == TOKEN_TYPE::OPAREN;
  bool test3 = tokens[8].type == TOKEN_TYPE::CPAREN &&
               tokens[9].type == TOKEN_TYPE::PLUS;
  print_test("Nested parentheses tokenized", test1 && test2 && test3);
}

void test_all_operators() {
  print_section("Lexer: All Operators");

  Lexer lexer("a|b*c+(@)#");
  auto tokens = lexer.tokenize();

  std::cout << YELLOW "Input: 'a|b*c+(@)#'" RESET << std::endl;
  print_tokens(tokens);

  int char_count = 0;
  int union_count = 0;
  int star_count = 0;
  int plus_count = 0;
  int lambda_count = 0;
  int empty_count = 0;

  for (const auto &token : tokens) {
    if (token.type == TOKEN_TYPE::LITERAL)
      char_count++;
    if (token.type == TOKEN_TYPE::UNION)
      union_count++;
    if (token.type == TOKEN_TYPE::STAR)
      star_count++;
    if (token.type == TOKEN_TYPE::PLUS)
      plus_count++;
    if (token.type == TOKEN_TYPE::LAMBDA)
      lambda_count++;
    if (token.type == TOKEN_TYPE::EMPTY)
      empty_count++;
  }

  bool test1 = char_count == 3; // a, b, c
  bool test2 = union_count == 1;
  bool test3 = star_count == 1;
  bool test4 = plus_count == 1;
  bool test5 = lambda_count == 1;
  bool test6 = empty_count == 1;

  print_test("All operators present",
             test1 && test2 && test3 && test4 && test5 && test6);
}

int main() {
  std::cout << CYAN "\n╔════════════════════════════════════════╗" RESET
            << std::endl;
  std::cout << CYAN "║       TEST SUITE - Lexer & Tokens      ║" RESET
            << std::endl;
  std::cout << CYAN "╚════════════════════════════════════════╝" RESET
            << std::endl;

  // Basic tests
  test_single_char();
  test_union_operator();
  test_star_operator();
  test_plus_operator();
  test_parentheses();
  test_lambda();
  test_empty();

  // Concatenation and complex
  test_simple_concat();
  test_complex_expression_1();
  test_complex_expression_2();
  test_with_lambda();

  // Edge cases
  test_digits_and_special();
  test_whitespace_handling();
  test_empty_input();
  test_nested_parentheses();
  test_all_operators();

  std::cout << "\n" CYAN "════════════════════════════════════════" RESET
            << std::endl;
  std::cout << GREEN "✓ Tests passed: " << tests_passed << RESET << std::endl;
  std::cout << RED "✗ Tests failed: " << tests_failed << RESET << std::endl;
  std::cout << CYAN "════════════════════════════════════════" RESET
            << std::endl;

  return tests_failed > 0 ? 1 : 0;
}
