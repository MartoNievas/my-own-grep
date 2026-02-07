#ifndef REG_HPP
#define REG_HPP

#include "dfa.hpp"
#include "fa.hpp"
#include "ndfa.hpp"
#include <memory>
#include <optional>
#include <string>

/* ABSTRACT CLASS FOR REGEX*/

class Regex {
protected:
  mutable DFA *_dfa_cache;

public:
  Regex() : _dfa_cache(nullptr) {};
  virtual ~Regex();

  DFA *dfa() const;
  bool match(const std::string &word) const;

  virtual NDFA *to_ndfa(void) const = 0;
  virtual bool _atomic(void) const = 0;
  virtual std::string to_string(void) const = 0;
};

/*CLASS - EMPTY LANGUAGE */

class Empty : public Regex {
public:
  NDFA *to_ndfa(void) const override;
  bool _atomic(void) const override;
  std::string to_string(void) const override;
};

/*CLASS - EMPTY STRING*/

class Lambda : public Regex {
public:
  NDFA *to_ndfa(void) const override;
  bool _atomic(void) const override;
  std::string to_string(void) const override;
};

/*CLASS - INDIVIDUAL CHARACTER*/

class Char : public Regex {
public:
  Char(char c);
  NDFA *to_ndfa(void) const override;
  bool _atomic(void) const override;
  std::string to_string(void) const override;
};

/*CONCAT - CONCATENATION FROM TWO REGEX*/

class Concat : public Regex {
private:
  std::shared_ptr<Regex> expr1;
  std::shared_ptr<Regex> expr2;

public:
  Concat(std::shared_ptr<Regex> e1, std::shared_ptr<Regex> e2)
      : expr1(e1), expr2(e2) {};
  NDFA *to_ndfa(void) const override;
  bool _atomic(void) const override;
  std::string to_string(void) const override;
};

#endif // !REG_HPP
