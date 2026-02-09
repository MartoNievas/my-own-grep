#ifndef REGEX_HPP
#define REGEX_HPP

#include "../automata/dfa.hpp"
#include "../automata/ndfa.hpp"
#include <memory>
#include <string>
namespace fa::regex {

/* ABSTRACT CLASS FOR REGEX*/

class Regex {
protected:
  mutable std::unique_ptr<DFA> _dfa_cache;

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
private:
  char symbol;

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
  Concat(std::shared_ptr<Regex> e1, std::shared_ptr<Regex> e2);
  NDFA *to_ndfa(void) const override;
  bool _atomic(void) const override;
  std::string to_string(void) const override;
};

/*CLASS - UNION FOR TWO REGEX*/

class Union : public Regex {
private:
  std::shared_ptr<Regex> expr1;
  std::shared_ptr<Regex> expr2;

public:
  Union(std::shared_ptr<Regex> e1, std::shared_ptr<Regex> e2);
  NDFA *to_ndfa(void) const override;
  bool _atomic(void) const override;
  std::string to_string(void) const override;
};

/*STAR - KLEEN CLOUSURE FOR A REGEX*/

class Star : public Regex {
private:
  std::shared_ptr<Regex> expr;

public:
  Star(std::shared_ptr<Regex> e);
  NDFA *to_ndfa(void) const override;
  bool _atomic(void) const override;
  std::string to_string(void) const override;
};

/*CLASS - PLUS FOR A REGEX */

class Plus : public Regex {
private:
  std::shared_ptr<Regex> expr;

public:
  Plus(std::shared_ptr<Regex> e);
  NDFA *to_ndfa(void) const override;
  bool _atomic(void) const override;
  std::string to_string(void) const override;
};
} // namespace fa::regex
#endif // !REGEX_HPP
