#include "../../include/fa/lexer/token.hpp"
#include <format>
#include <string>

using namespace std;
using namespace fa::regex;

string Token::to_string() const {
  switch (type) {
  case TOKEN_TYPE::LITERAL: {
    switch (value) {
    case '\0':
      return "LITERAL(\\0)";
    case '\n':
      return "LITERAL(\\n)";
    case '\t':
      return "LITERAL(\\t)";
    case '\r':
      return "LITERAL(\\r)";
    case ' ':
      return "LITERAL(' ')";
    default:
      return format("LITERAL('{}')", value);
    }
  }

  case TOKEN_TYPE::UNION:
    return "UNION(|)";
  case TOKEN_TYPE::STAR:
    return "STAR(*)";
  case TOKEN_TYPE::PLUS:
    return "PLUS(+)";
  case TOKEN_TYPE::CONCAT:
    return "CONCAT(.)"; // Importante para el Parser
  case TOKEN_TYPE::OPAREN:
    return "OPAREN(()";
  case TOKEN_TYPE::CPAREN:
    return "CPAREN())";
  case TOKEN_TYPE::LAMBDA:
    return "LAMBDA(ε)";
  case TOKEN_TYPE::EMPTY:
    return "EMPTY(∅)";
  case TOKEN_TYPE::END:
    return "EOF";
  case TOKEN_TYPE::INVALID:
    return "INVALID_TOKEN";

  default:
    return format("UNKNOWN_TOKEN({})", static_cast<int>(type));
  }
}
