#include "../../include/fa/lexer/lexer.hpp"
#include "../../include/fa/lexer/token.hpp"
#include "../../include/fa/regex/regex.hpp"
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

char Lexer::get_escaped_char() {
  char c = current_char;
  if (c == '/') {
    advance();
    if (current_char == '\0')
      return '\0';

    switch (current_char) {
    case 'n':
      c = '\n';
      break;
    case 't':
      c = '\t';
      break;
    case 'r':
      c = '\r';
      break;
    default:
      c = current_char;
      break;
    }
    advance();
  } else {
    advance();
  }
  return c;
}

Token Lexer::next_token() {
  if (current_char == '\0')
    return Token(TOKEN_TYPE::END);

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

  case '[': {
    advance();
    bool negate = false;
    if (current_char == '^') {
      negate = true;
      advance();
    }

    fa::regex::CharClass cls;
    cls.negate = negate;

    if (current_char == ']') {
      cls.add_literal(']');
      advance();
    }

    while (current_char != ']' && current_char != '\0') {
      char start_c = get_escaped_char();
      if (start_c == '\0')
        return Token(TOKEN_TYPE::INVALID);

      if (current_char == '-' && peek(1) != ']' && peek(1) != '\0') {
        advance();
        char end_c = get_escaped_char();
        if (end_c == '\0')
          return Token(TOKEN_TYPE::INVALID);

        cls.add_range((unsigned char)start_c, (unsigned char)end_c);
      } else {
        cls.add_literal((unsigned char)start_c);
      }
    }

    if (current_char != ']')
      return Token(TOKEN_TYPE::INVALID);

    advance();

    Token t(TOKEN_TYPE::RANGE);
    t.set_char_class(cls);
    if (cls.bits.none()) {
      return Token(TOKEN_TYPE::INVALID);
    }
    return t;
  }

  case '/': {
    char escaped = get_escaped_char();
    if (escaped == '\0')
      return Token(TOKEN_TYPE::INVALID);
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
    if (token.get_type() == TOKEN_TYPE::INVALID)
      throw invalid_argument("Lexer Error: Invalid character at position " +
                             to_string(position));

    tokens.push_back(token);
    if (token.get_type() == TOKEN_TYPE::END)
      break;
  }
  return insert_implicit_concatenation(tokens);
}

vector<Token>
Lexer::insert_implicit_concatenation(const vector<Token> &tokens) {
  vector<Token> result;
  for (size_t i = 0; i < tokens.size(); ++i) {
    Token t1 = tokens[i];
    result.push_back(t1);

    if (i + 1 < tokens.size()) {
      Token t2 = tokens[i + 1];

      bool t1_is_operand = (t1.get_type() == TOKEN_TYPE::LITERAL ||
                            t1.get_type() == TOKEN_TYPE::CPAREN ||
                            t1.get_type() == TOKEN_TYPE::STAR ||
                            t1.get_type() == TOKEN_TYPE::PLUS ||
                            t1.get_type() == TOKEN_TYPE::RANGE);

      bool t2_starts_operand = (t2.get_type() == TOKEN_TYPE::LITERAL ||
                                t2.get_type() == TOKEN_TYPE::OPAREN ||
                                t2.get_type() == TOKEN_TYPE::RANGE);

      if (t1_is_operand && t2_starts_operand)
        result.push_back(Token(TOKEN_TYPE::CONCAT, '.'));
    }
  }
  return result;
}
