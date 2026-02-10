#include "../../include/fa/parser/parser.hpp"
#include "../../include/fa/lexer/token.hpp"
#include "../../include/fa/regex/regex.hpp"
#include <format>
#include <memory>
#include <stdexcept>

using namespace std;
using namespace fa::regex;

// public methods

shared_ptr<Regex> Parser::parse(void) {
  auto result = parse_union();
  if (current.type != TOKEN_TYPE::END) {
    throw runtime_error("Error: Unexpected character at the end of expression");
  }
  return result;
}

// Private methods

void Parser::eat(TOKEN_TYPE type) {
  if (current.type == type) {
    current = lexer.next_token();
  } else {
    throw runtime_error(
        format("Syntax Error: Expected token type {}, but found {}", (int)type,
               current.to_string()));
  }
}

shared_ptr<Regex> Parser::parse_union(void) {
  auto expr = parse_concat();

  while (current.type == TOKEN_TYPE::UNION) {
    eat(TOKEN_TYPE::UNION);
    auto right = parse_concat();
    expr = make_shared<Union>(expr, right);
  }
  return expr;
}

shared_ptr<Regex> Parser::parse_concat(void) {
  auto expr = parse_star();

  while (current.type == TOKEN_TYPE::LITERAL ||
         current.type == TOKEN_TYPE::OPAREN ||
         current.type == TOKEN_TYPE::LAMBDA ||
         current.type == TOKEN_TYPE::EMPTY) {
    auto next_expr = parse_star();
    expr = make_shared<Concat>(expr, next_expr);
  }
  return expr;
}

shared_ptr<Regex> Parser::parse_star(void) {
  auto expr = parse_atom();

  while (current.type == TOKEN_TYPE::STAR || current.type == TOKEN_TYPE::PLUS) {
    if (current.type == TOKEN_TYPE::STAR) {
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
  switch (current.type) {
  case TOKEN_TYPE::LITERAL: {
    char c = current.value;
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
  default:
    throw runtime_error(format("Error: Unknown atom or empty expression: {}",
                               current.to_string()));
  }
}
