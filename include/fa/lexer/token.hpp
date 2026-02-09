#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
typedef enum {
  LITERAL,

  // Operators
  UNION,
  STAR,
  PLUS,
  CONCAT,

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
public:
  TOKEN_TYPE type;
  char value;

  Token(TOKEN_TYPE type, char literal = '\0') : type(type), value(literal) {}

  std::string to_string() const;
};
#endif // !TOKEN_HPP
#define TOKEN_HPP
