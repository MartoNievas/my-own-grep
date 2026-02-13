#include "../../include/fa/parser/parser.hpp"
#include "../../include/fa/lexer/token.hpp"
#include "../../include/fa/regex/regex.hpp"
#include <format>
#include <memory>
#include <stdexcept>

using namespace std;
using namespace fa::regex;

Parser::Parser(const string &input)
    : current_idx(0), current(TOKEN_TYPE::INVALID) {
  Lexer lexer(input);
  tokens = lexer.tokenize();
  if (!tokens.empty()) {
    current = tokens[current_idx];
  }
}

shared_ptr<Regex> Parser::parse(void) {
  auto result = parse_union();
  if (current.get_type() != TOKEN_TYPE::END) {
    throw runtime_error("Error: Unexpected character at the end of expression");
  }
  return result;
}

void Parser::eat(TOKEN_TYPE type) {
  if (current.get_type() == type) {
    current_idx++;
    if (current_idx < tokens.size()) {
      current = tokens[current_idx];
    }
  } else {
    throw runtime_error(
        format("Syntax Error: Expected token type {}, but found {}", (int)type,
               current.to_string()));
  }
}

shared_ptr<Regex> Parser::parse_union(void) {
  auto expr = parse_concat();
  while (current.get_type() == TOKEN_TYPE::UNION) {
    eat(TOKEN_TYPE::UNION);
    auto right = parse_concat();
    expr = make_shared<Union>(expr, right);
  }
  return expr;
}

shared_ptr<Regex> Parser::parse_concat(void) {
  auto expr = parse_star();
  while (current.get_type() == TOKEN_TYPE::CONCAT) {
    eat(TOKEN_TYPE::CONCAT);
    auto next_expr = parse_star();
    expr = make_shared<Concat>(expr, next_expr);
  }
  return expr;
}

shared_ptr<Regex> Parser::parse_star(void) {
  auto expr = parse_atom();
  while (current.get_type() == TOKEN_TYPE::STAR ||
         current.get_type() == TOKEN_TYPE::PLUS) {
    if (current.get_type() == TOKEN_TYPE::STAR) {
      eat(TOKEN_TYPE::STAR);
      expr = make_shared<Star>(expr);
    } else {
      eat(TOKEN_TYPE::PLUS);
      expr = make_shared<Plus>(expr);
    }
  }
  return expr;
}

shared_ptr<Regex> Parser::parse_atom(void) {
  switch (current.get_type()) {
  case TOKEN_TYPE::LITERAL: {
    char c = current.get_value();
    eat(TOKEN_TYPE::LITERAL);
    return make_shared<Char>(c);
  }
  case TOKEN_TYPE::OPAREN: {
    eat(TOKEN_TYPE::OPAREN);
    auto expr = parse_union();
    eat(TOKEN_TYPE::CPAREN);
    return expr;
  }
  case TOKEN_TYPE::LAMBDA: {
    eat(TOKEN_TYPE::LAMBDA);
    return make_shared<Lambda>();
  }
  case TOKEN_TYPE::EMPTY: {
    eat(TOKEN_TYPE::EMPTY);
    return make_shared<Empty>();
  }
  case TOKEN_TYPE::RANGE: {
    auto node = make_shared<fa::regex::Range>(current.get_char_class());
    eat(TOKEN_TYPE::RANGE);
    return node;
  }
  default:
    throw runtime_error(format("Error: Unknown atom or empty expression: {}",
                               current.to_string()));
  }
}
