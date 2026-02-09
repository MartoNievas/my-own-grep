#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <string>
#include <vector>

class Lexer {
public:
  std::string input;
  size_t position;
  char current_char;

  void advance(void);
  void skip_whitespace(void);
  char peek(int offset = 1) const;

public:
  Lexer(const std::string &text);

  Token next_token(void);
  std::vector<Token> tokenize(void);
};

#endif // !LEXER_HPP
