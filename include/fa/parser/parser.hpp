#ifndef PARSER_HPP
#define PARSER_HPP

#include "../lexer/lexer.hpp"
#include "../lexer/token.hpp"
#include "../regex/regex.hpp"
#include <memory>
#include <string>
class Parser {
private:
  Lexer lexer;
  Token current;

public:
  Parser(const std::string &input)
      : lexer(input), current(lexer.next_token()) {}
  std::shared_ptr<fa::regex::Regex> parse(void);
  void eat(TOKEN_TYPE type);
  std::shared_ptr<fa::regex::Regex> parse_union(void);
  std::shared_ptr<fa::regex::Regex> parse_concat(void);
  std::shared_ptr<fa::regex::Regex> parse_star(void);
  std::shared_ptr<fa::regex::Regex> parse_atom(void);
};

#endif
