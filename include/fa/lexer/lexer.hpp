#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <string_view>
#include <vector>

class Lexer {
public:
  explicit Lexer(std::string_view text);
  std::vector<Token> tokenize();

  std::string_view input;
  size_t position;
  char current_char;

  void advance();
  char peek(int offset) const;
  Token next_token();

  char get_escaped_char();

  std::vector<Token>
  insert_implicit_concatenation(const std::vector<Token> &tokens);
};

#endif
