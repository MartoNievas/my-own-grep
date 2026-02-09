#include "../../include/fa/lexer/token.hpp"

using namespace std;

string Token::to_string() const {
  switch (type) {
  case TOKEN_TYPE::LITERAL:
    if (value == '\0') {
      return "CHAR('\\0')"; // Mostrar explícitamente
    }
    return string("CHAR('") + value + "')";

  case TOKEN_TYPE::UNION:
    return "UNION(|)";

  case TOKEN_TYPE::STAR:
    return "STAR(*)";

  case TOKEN_TYPE::PLUS:
    return "PLUS(+)";

  case TOKEN_TYPE::OPAREN:
    return "OPAREN(()";

  case TOKEN_TYPE::CPAREN:
    return "CPAREN())";

  case TOKEN_TYPE::LAMBDA:
    return "LAMBDA(ε)";

  case TOKEN_TYPE::EMPTY:
    return "EMPTY(∅)";

  case TOKEN_TYPE::END:
    return "END_OF_INPUT";

  case TOKEN_TYPE::INVALID:
    return "INVALID";

  default:
    return "UNKNOWN";
  }
}
