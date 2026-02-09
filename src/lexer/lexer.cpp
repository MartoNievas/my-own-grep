#include "../../include/fa/lexer/lexer.hpp"
#include "../../include/fa/lexer/token.hpp"
#include <cctype>
#include <stdexcept>

using namespace std;

Lexer::Lexer(const string &text) : input(text), position(0) {
  current_char = input.empty() ? '\0' : input[0];
}

void Lexer::advance(void) {
  position++;
  current_char = (position < input.length()) ? input[position] : '\0';
}

void Lexer::skip_whitespace(void) {
  while (current_char != '\0' && isspace(current_char)) {
    advance();
  }
}

char Lexer::peek(int offset) const {
  size_t peek_pos = position + offset;
  return (peek_pos < input.length()) ? input[peek_pos] : '\0';
}

Token Lexer::next_token(void) {
  skip_whitespace();

  if (current_char == '\0') {
    return Token(TOKEN_TYPE::END);
  }

  switch (current_char) {
  case '|':
    advance();
    return Token(TOKEN_TYPE::UNION, '|');

  case '*':
    advance();
    return Token(TOKEN_TYPE::STAR, '*');

  case '+':
    advance();
    return Token(TOKEN_TYPE::PLUS, '+');

  case '(':
    advance();
    return Token(TOKEN_TYPE::OPAREN, '(');

  case ')':
    advance();
    return Token(TOKEN_TYPE::CPAREN, ')');

  case '@':
    advance();
    return Token(TOKEN_TYPE::LAMBDA);

  case '#':
    advance();
    return Token(TOKEN_TYPE::EMPTY);

  case '\\': {
    advance();
    if (current_char == '\0') {
      return Token(TOKEN_TYPE::INVALID);
    }
    char escaped = current_char;
    advance();
    return Token(TOKEN_TYPE::LITERAL, escaped);
  }

  default:
    if (isalnum(current_char)) {
      char c = current_char;
      advance();
      return Token(TOKEN_TYPE::LITERAL, c);
    }
    advance();
    return Token(TOKEN_TYPE::INVALID);
  }
}

vector<Token> Lexer::tokenize(void) {
  vector<Token> tokens;
  Token token = next_token();

  while (token.type != TOKEN_TYPE::END) {
    if (token.type == TOKEN_TYPE::INVALID) {
      throw invalid_argument("This token is invalid");
    }
    tokens.push_back(token);
    token = next_token();
  }

  tokens.push_back(token);
  return tokens;
}
