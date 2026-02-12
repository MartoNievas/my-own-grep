#include "../../include/fa/lexer/lexer.hpp"
#include "../../include/fa/lexer/token.hpp"
#include <cctype>
#include <stdexcept>
#include <string_view>

using namespace std;

Lexer::Lexer(string_view text) : input(text), position(0) {
  current_char = input.empty() ? '\0' : input[0];
}

void Lexer::advance() {
  position++;
  current_char = (position < input.length()) ? input[position] : '\0';
}

char Lexer::peek(int offset) const {
  size_t peek_pos = position + offset;
  return (peek_pos < input.length()) ? input[peek_pos] : '\0';
}

void Lexer::skip_whitespace() {
  while (current_char != '\0' && isspace(current_char)) {
    advance();
  }
}

Token Lexer::next_token() {
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

  case '\\': { // Manejo de escapes mejorado
    advance();
    if (current_char == '\0')
      return Token(TOKEN_TYPE::INVALID);

    char escaped = current_char;
    switch (current_char) {
    case 'n':
      escaped = '\n';
      break;
    case 't':
      escaped = '\t';
      break;
    case 'r':
      escaped = '\r';
      break;
    }
    advance();
    return Token(TOKEN_TYPE::LITERAL, escaped);
  }

  default: {
    char c = current_char;
    advance();
    return Token(TOKEN_TYPE::LITERAL, c);
  }
  }
}

vector<Token> Lexer::tokenize() {
  vector<Token> tokens;

  while (true) {
    Token token = next_token();
    if (token.type == TOKEN_TYPE::INVALID) {
      throw invalid_argument("Lexer Error: Invalid character at position " +
                             to_string(position));
    }

    tokens.push_back(token);
    if (token.type == TOKEN_TYPE::END)
      break;
  }

  return insert_implicit_concatenation(tokens);
}

// Función auxiliar opcional pero MUY recomendada para Regex
vector<Token>
Lexer::insert_implicit_concatenation(const vector<Token> &tokens) {
  vector<Token> result;
  for (size_t i = 0; i < tokens.size(); ++i) {
    Token t1 = tokens[i];
    result.push_back(t1);

    if (i + 1 < tokens.size()) {
      Token t2 = tokens[i + 1];

      // Lógica: Si t1 es un operando y t2 es el inicio de otro operando,
      // insertar CONCAT
      bool t1_is_operand =
          (t1.type == TOKEN_TYPE::LITERAL || t1.type == TOKEN_TYPE::CPAREN ||
           t1.type == TOKEN_TYPE::STAR || t1.type == TOKEN_TYPE::PLUS ||
           t1.type == TOKEN_TYPE::LAMBDA || t1.type == TOKEN_TYPE::EMPTY);

      bool t2_starts_operand =
          (t2.type == TOKEN_TYPE::LITERAL || t2.type == TOKEN_TYPE::OPAREN ||
           t2.type == TOKEN_TYPE::LAMBDA || t2.type == TOKEN_TYPE::EMPTY);

      if (t1_is_operand && t2_starts_operand) {
        // Debes tener un tipo TOKEN_TYPE::CONCAT en tu enum
        result.push_back(Token(TOKEN_TYPE::CONCAT, '.'));
      }
    }
  }
  return result;
}
