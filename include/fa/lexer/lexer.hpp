#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include <string>
#include <string_view>
#include <vector>

class Lexer {
public:
  std::string input;
  size_t position;
  char current_char;

  void advance(void);
  char peek(int offset = 1) const;

public:
  Lexer(std::string_view text);

  Token next_token(void);
  std::vector<Token> tokenize(void);

private:
  std::vector<Token>
  insert_implicit_concatenation(const std::vector<Token> &tokens);
};

#endif // !LEXER_HPP
