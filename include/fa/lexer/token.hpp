#ifndef TOKEN_HPP
#define TOKEN_HPP
#include "../regex/regex.hpp"
#include <string>
typedef enum {
  LITERAL,

  // Operators
  UNION,
  STAR,
  PLUS,
  CONCAT,
  RANGE,

  // Groups
  OPAREN,
  CPAREN,

  // ESPECIALS
  LAMBDA,
  EMPTY,

  // CONTROL
  END,
  INVALID
} TOKEN_TYPE;

class Token {
private:
  TOKEN_TYPE type;
  char value;

  fa::regex::CharClass char_class;

public:
  Token(TOKEN_TYPE type, char literal = '\0') : type(type), value(literal) {}

  // Getters
  TOKEN_TYPE get_type() const { return type; }
  char get_value() const { return value; }
  const fa::regex::CharClass &get_char_class() const { return char_class; }

  // Setters
  void set_type(TOKEN_TYPE t) { type = t; }
  void set_value(char v) { value = v; }

  void set_char_class(const fa::regex::CharClass &cls) {
    this->char_class = cls;
  }

  void set_negate(bool n) { char_class.negate = n; }

  std::string to_string() const;
};
#endif // !TOKEN_HPP
