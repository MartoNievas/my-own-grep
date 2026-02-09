#include "../../include/fa/automata/dfa.hpp"
#include "../../include/fa/automata/ndfa.hpp"
#include "../../include/fa/regex/regex.hpp"
#include <iostream>
#include <memory>
#include <set>

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

using namespace std;
using namespace fa::regex;

void test_empty_basic() {
  print_section("Empty: Basic Tests");
  Empty empty;
  print_test("Empty rejects empty string", !empty.match(""));
  print_test("Empty rejects 'a'", !empty.match("a"));
  print_test("Empty rejects 'abc'", !empty.match("abc"));
  print_test("Empty to_string is '∅'", empty.to_string() == "∅");
  print_test("Empty is atomic", empty._atomic());
}

void test_lambda_basic() {
  print_section("Lambda: Basic Tests");
  Lambda lambda;
  print_test("Lambda accepts empty string", lambda.match(""));
  print_test("Lambda rejects 'a'", !lambda.match("a"));
  print_test("Lambda rejects 'abc'", !lambda.match("abc"));
  print_test("Lambda to_string is 'λ'", lambda.to_string() == "λ");
  print_test("Lambda is atomic", lambda._atomic());
}

void test_char_basic() {
  print_section("Char: Basic Tests");
  Char char_a('a');
  print_test("Char('a') accepts 'a'", char_a.match("a"));
  print_test("Char('a') rejects empty", !char_a.match(""));
  print_test("Char('a') rejects 'b'", !char_a.match("b"));
  print_test("Char('a') rejects 'aa'", !char_a.match("aa"));
  print_test("Char('a') to_string is 'a'", char_a.to_string() == "a");
  print_test("Char is atomic", char_a._atomic());
}

void test_char_multiple() {
  print_section("Char: Multiple Characters");
  Char char_x('x');
  Char char_0('0');
  print_test("Char('x') accepts 'x'", char_x.match("x"));
  print_test("Char('x') rejects 'y'", !char_x.match("y"));
  print_test("Char('0') accepts '0'", char_0.match("0"));
  print_test("Char('0') rejects '1'", !char_0.match("1"));
}

void test_concat_simple() {
  print_section("Concat: Simple Cases");
  auto a = make_shared<Char>('a');
  auto b = make_shared<Char>('b');
  Concat ab(a, b);
  print_test("Concat(a,b) accepts 'ab'", ab.match("ab"));
  print_test("Concat(a,b) rejects empty", !ab.match(""));
  print_test("Concat(a,b) rejects 'a'", !ab.match("a"));
  print_test("Concat(a,b) rejects 'b'", !ab.match("b"));
  print_test("Concat(a,b) rejects 'ba'", !ab.match("ba"));
  print_test("Concat(a,b) rejects 'aab'", !ab.match("aab"));
  print_test("Concat is not atomic", !ab._atomic());
}

void test_concat_with_lambda() {
  print_section("Concat: With Lambda");
  auto a = make_shared<Char>('a');
  auto lambda = make_shared<Lambda>();
  Concat a_lambda(a, lambda);
  print_test("Concat(a,λ) accepts 'a'", a_lambda.match("a"));
  print_test("Concat(a,λ) rejects empty", !a_lambda.match(""));
  print_test("Concat(a,λ) rejects 'aa'", !a_lambda.match("aa"));
}

void test_concat_chain() {
  print_section("Concat: Chain Multiple");
  auto a = make_shared<Char>('a');
  auto b = make_shared<Char>('b');
  auto c = make_shared<Char>('c');
  auto ab = make_shared<Concat>(a, b);
  Concat abc(ab, c);
  print_test("Concat(Concat(a,b),c) accepts 'abc'", abc.match("abc"));
  print_test("Concat(Concat(a,b),c) rejects 'ab'", !abc.match("ab"));
  print_test("Concat(Concat(a,b),c) rejects 'bc'", !abc.match("bc"));
}

void test_union_simple() {
  print_section("Union: Simple Cases");
  auto a = make_shared<Char>('a');
  auto b = make_shared<Char>('b');
  Union a_or_b(a, b);
  print_test("Union(a,b) accepts 'a'", a_or_b.match("a"));
  print_test("Union(a,b) accepts 'b'", a_or_b.match("b"));
  print_test("Union(a,b) rejects empty", !a_or_b.match(""));
  print_test("Union(a,b) rejects 'c'", !a_or_b.match("c"));
  print_test("Union(a,b) rejects 'ab'", !a_or_b.match("ab"));
  print_test("Union is not atomic", !a_or_b._atomic());
}

void test_union_with_lambda() {
  print_section("Union: With Lambda");
  auto a = make_shared<Char>('a');
  auto lambda = make_shared<Lambda>();
  Union a_or_lambda(a, lambda);
  print_test("Union(a,λ) accepts 'a'", a_or_lambda.match("a"));
  print_test("Union(a,λ) accepts empty", a_or_lambda.match(""));
  print_test("Union(a,λ) rejects 'b'", !a_or_lambda.match("b"));
}

void test_union_complex() {
  print_section("Union: Complex Patterns");
  auto a = make_shared<Char>('a');
  auto b = make_shared<Char>('b');
  auto c = make_shared<Char>('c');
  auto ab = make_shared<Concat>(a, b);
  Union ab_or_c(ab, c);
  print_test("Union(ab,c) accepts 'ab'", ab_or_c.match("ab"));
  print_test("Union(ab,c) accepts 'c'", ab_or_c.match("c"));
  print_test("Union(ab,c) rejects 'a'", !ab_or_c.match("a"));
  print_test("Union(ab,c) rejects 'b'", !ab_or_c.match("b"));
}

void test_star_simple() {
  print_section("Star: Simple Cases");
  auto a = make_shared<Char>('a');
  Star a_star(a);
  print_test("Star(a) accepts empty", a_star.match(""));
  print_test("Star(a) accepts 'a'", a_star.match("a"));
  print_test("Star(a) accepts 'aa'", a_star.match("aa"));
  print_test("Star(a) accepts 'aaa'", a_star.match("aaa"));
  print_test("Star(a) accepts 'aaaa'", a_star.match("aaaa"));
  print_test("Star(a) rejects 'b'", !a_star.match("b"));
  print_test("Star(a) rejects 'ab'", !a_star.match("ab"));
  print_test("Star is not atomic", !a_star._atomic());
}

void test_star_pattern() {
  print_section("Star: Pattern (ab)*");
  auto a = make_shared<Char>('a');
  auto b = make_shared<Char>('b');
  auto ab = make_shared<Concat>(a, b);
  Star ab_star(ab);
  print_test("Star(ab) accepts empty", ab_star.match(""));
  print_test("Star(ab) accepts 'ab'", ab_star.match("ab"));
  print_test("Star(ab) accepts 'abab'", ab_star.match("abab"));
  print_test("Star(ab) accepts 'ababab'", ab_star.match("ababab"));
  print_test("Star(ab) rejects 'a'", !ab_star.match("a"));
  print_test("Star(ab) rejects 'aba'", !ab_star.match("aba"));
  print_test("Star(ab) rejects 'aab'", !ab_star.match("aab"));
}

void test_plus_simple() {
  print_section("Plus: Simple Cases");
  auto a = make_shared<Char>('a');
  Plus a_plus(a);
  print_test("Plus(a) rejects empty", !a_plus.match(""));
  print_test("Plus(a) accepts 'a'", a_plus.match("a"));
  print_test("Plus(a) accepts 'aa'", a_plus.match("aa"));
  print_test("Plus(a) accepts 'aaa'", a_plus.match("aaa"));
  print_test("Plus(a) rejects 'b'", !a_plus.match("b"));
  print_test("Plus is not atomic", !a_plus._atomic());
}

void test_plus_pattern() {
  print_section("Plus: Pattern (ab)+");
  auto a = make_shared<Char>('a');
  auto b = make_shared<Char>('b');
  auto ab = make_shared<Concat>(a, b);
  Plus ab_plus(ab);
  print_test("Plus(ab) rejects empty", !ab_plus.match(""));
  print_test("Plus(ab) accepts 'ab'", ab_plus.match("ab"));
  print_test("Plus(ab) accepts 'abab'", ab_plus.match("abab"));
  print_test("Plus(ab) accepts 'ababab'", ab_plus.match("ababab"));
  print_test("Plus(ab) rejects 'a'", !ab_plus.match("a"));
}

void test_complex_pattern_1() {
  print_section("Complex: (a|b)*");
  auto a = make_shared<Char>('a');
  auto b = make_shared<Char>('b');
  auto a_or_b = make_shared<Union>(a, b);
  Star pattern(a_or_b);
  print_test("(a|b)* accepts empty", pattern.match(""));
  print_test("(a|b)* accepts 'a'", pattern.match("a"));
  print_test("(a|b)* accepts 'b'", pattern.match("b"));
  print_test("(a|b)* accepts 'ab'", pattern.match("ab"));
  print_test("(a|b)* accepts 'ba'", pattern.match("ba"));
  print_test("(a|b)* accepts 'aabb'", pattern.match("aabb"));
  print_test("(a|b)* accepts 'abba'", pattern.match("abba"));
  print_test("(a|b)* rejects 'c'", !pattern.match("c"));
  print_test("(a|b)* rejects 'abc'", !pattern.match("abc"));
}

void test_complex_pattern_2() {
  print_section("Complex: a(b|c)*d");
  auto a = make_shared<Char>('a');
  auto b = make_shared<Char>('b');
  auto c = make_shared<Char>('c');
  auto d = make_shared<Char>('d');
  auto b_or_c = make_shared<Union>(b, c);
  auto b_or_c_star = make_shared<Star>(b_or_c);
  auto a_bc_star = make_shared<Concat>(a, b_or_c_star);
  Concat pattern(a_bc_star, d);
  print_test("a(b|c)*d accepts 'ad'", pattern.match("ad"));
  print_test("a(b|c)*d accepts 'abd'", pattern.match("abd"));
  print_test("a(b|c)*d accepts 'acd'", pattern.match("acd"));
  print_test("a(b|c)*d accepts 'abcd'", pattern.match("abcd"));
  print_test("a(b|c)*d accepts 'abbd'", pattern.match("abbd"));
  print_test("a(b|c)*d accepts 'accd'", pattern.match("accd"));
  print_test("a(b|c)*d rejects 'a'", !pattern.match("a"));
  print_test("a(b|c)*d rejects 'd'", !pattern.match("d"));
  print_test("a(b|c)*d rejects 'ab'", !pattern.match("ab"));
}

void test_complex_pattern_3() {
  print_section("Complex: (a+b+)+");
  auto a = make_shared<Char>('a');
  auto b = make_shared<Char>('b');
  auto a_plus = make_shared<Plus>(a);
  auto b_plus = make_shared<Plus>(b);
  auto a_plus_b_plus = make_shared<Concat>(a_plus, b_plus);
  Plus pattern(a_plus_b_plus);
  print_test("(a+b+)+ accepts 'ab'", pattern.match("ab"));
  print_test("(a+b+)+ accepts 'aabb'", pattern.match("aabb"));
  print_test("(a+b+)+ accepts 'aaabbb'", pattern.match("aaabbb"));
  print_test("(a+b+)+ accepts 'ababab'", pattern.match("ababab"));
  print_test("(a+b+)+ accepts 'aabbaabb'", pattern.match("aabbaabb"));
  print_test("(a+b+)+ rejects empty", !pattern.match(""));
  print_test("(a+b+)+ rejects 'a'", !pattern.match("a"));
  print_test("(a+b+)+ rejects 'b'", !pattern.match("b"));
  print_test("(a+b+)+ rejects 'ba'", !pattern.match("ba"));
}

void test_complex_pattern_4() {
  print_section("Complex: (0|1)*00(0|1)*");
  auto zero = make_shared<Char>('0');
  auto one = make_shared<Char>('1');
  auto zero_or_one = make_shared<Union>(zero, one);
  auto prefix = make_shared<Star>(zero_or_one);
  auto double_zero1 = make_shared<Concat>(zero, zero);
  auto middle = make_shared<Concat>(prefix, double_zero1);
  auto zero_or_one2 =
      make_shared<Union>(make_shared<Char>('0'), make_shared<Char>('1'));
  auto suffix = make_shared<Star>(zero_or_one2);
  Concat pattern(middle, suffix);
  print_test("Pattern accepts '00'", pattern.match("00"));
  print_test("Pattern accepts '000'", pattern.match("000"));
  print_test("Pattern accepts '100'", pattern.match("100"));
  print_test("Pattern accepts '001'", pattern.match("001"));
  print_test("Pattern accepts '1001'", pattern.match("1001"));
  print_test("Pattern rejects '0'", !pattern.match("0"));
  print_test("Pattern rejects '01'", !pattern.match("01"));
  print_test("Pattern rejects '11'", !pattern.match("11"));
}

void test_edge_cases() {
  print_section("Edge Cases");
  auto empty = make_shared<Empty>();
  auto a = make_shared<Char>('a');
  Concat empty_a(empty, a);
  print_test("Empty concat a rejects 'a'", !empty_a.match("a"));
  print_test("Empty concat a rejects empty", !empty_a.match(""));
  auto lambda = make_shared<Lambda>();
  Star lambda_star(lambda);
  print_test("Lambda* accepts empty", lambda_star.match(""));
  print_test("Lambda* rejects 'a'", !lambda_star.match("a"));
  Plus lambda_plus(lambda);
  print_test("Lambda+ accepts empty", lambda_plus.match(""));
  print_test("Lambda+ rejects 'a'", !lambda_plus.match("a"));
}

void test_nested_operators() {
  print_section("Nested Operators");
  auto a = make_shared<Char>('a');
  auto a_star = make_shared<Star>(a);
  Star a_star_star(a_star);
  print_test("(a*)* accepts empty", a_star_star.match(""));
  print_test("(a*)* accepts 'a'", a_star_star.match("a"));
  print_test("(a*)* accepts 'aaa'", a_star_star.match("aaa"));
  auto b = make_shared<Char>('b');
  auto a_or_b = make_shared<Union>(a, b);
  auto a_or_b_plus = make_shared<Plus>(a_or_b);
  Plus nested_plus(a_or_b_plus);
  print_test("((a|b)+)+ accepts 'a'", nested_plus.match("a"));
  print_test("((a|b)+)+ accepts 'ab'", nested_plus.match("ab"));
  print_test("((a|b)+)+ rejects empty", !nested_plus.match(""));
}

int main() {
  std::cout << CYAN "\n╔════════════════════════════════════════╗" RESET
            << std::endl;
  std::cout << CYAN "║      TEST SUITE - Regular Expressions    ║" RESET
            << std::endl;
  std::cout << CYAN "╚════════════════════════════════════════╝" RESET
            << std::endl;

  test_empty_basic();
  test_lambda_basic();
  test_char_basic();
  test_char_multiple();
  test_concat_simple();
  test_concat_with_lambda();
  test_concat_chain();
  test_union_simple();
  test_union_with_lambda();
  test_union_complex();
  test_star_simple();
  test_star_pattern();
  test_plus_simple();
  test_plus_pattern();
  test_complex_pattern_1();
  test_complex_pattern_2();
  test_complex_pattern_3();
  test_complex_pattern_4();
  test_edge_cases();
  test_nested_operators();

  std::cout << "\n" CYAN "════════════════════════════════════════" RESET
            << std::endl;
  std::cout << GREEN "✓ Tests passed: " << tests_passed << RESET << std::endl;
  std::cout << RED "✗ Tests failed: " << tests_failed << RESET << std::endl;
  std::cout << CYAN "════════════════════════════════════════" RESET
            << std::endl;

  return tests_failed > 0 ? 1 : 0;
}
