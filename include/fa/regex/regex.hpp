#ifndef REGEX_HPP
#define REGEX_HPP
#include "../automata/dfa.hpp"
#include "../automata/ndfa.hpp"
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace fa::regex {
struct DFA_Fast {
  int initial_state = -1;
  std::vector<std::array<int, 256>> transitions;
  std::vector<bool> accept_states;
};
class Regex {

protected:
  mutable std::unique_ptr<DFA> _dfa_cache;
  mutable std::unique_ptr<DFA_Fast> _dfa_fast_cache;

public:
  Regex() : _dfa_cache(nullptr) {}
  virtual ~Regex();

  const DFA *dfa() const;

  bool match(const std::string &word) const;

  virtual std::unique_ptr<NDFA> to_ndfa() const = 0;
  virtual bool _atomic() const = 0;
  virtual std::string to_string() const = 0;
};

class Empty : public Regex {
public:
  std::unique_ptr<NDFA> to_ndfa() const override;
  bool _atomic() const override;
  std::string to_string() const override;
};

class Lambda : public Regex {
public:
  std::unique_ptr<NDFA> to_ndfa() const override;
  bool _atomic() const override;
  std::string to_string() const override;
};

class Char : public Regex {
private:
  char symbol;

public:
  explicit Char(char c);
  std::unique_ptr<NDFA> to_ndfa() const override;
  bool _atomic() const override;
  std::string to_string() const override;
};

class Concat : public Regex {
private:
  std::shared_ptr<Regex> expr1;
  std::shared_ptr<Regex> expr2;

public:
  Concat(std::shared_ptr<Regex> e1, std::shared_ptr<Regex> e2);
  std::unique_ptr<NDFA> to_ndfa() const override;
  bool _atomic() const override;
  std::string to_string() const override;
};

class Union : public Regex {
private:
  std::shared_ptr<Regex> expr1;
  std::shared_ptr<Regex> expr2;

public:
  Union(std::shared_ptr<Regex> e1, std::shared_ptr<Regex> e2);
  std::unique_ptr<NDFA> to_ndfa() const override;
  bool _atomic() const override;
  std::string to_string() const override;
};

class Star : public Regex {
private:
  std::shared_ptr<Regex> expr;

public:
  explicit Star(std::shared_ptr<Regex> e);
  std::unique_ptr<NDFA> to_ndfa() const override;
  bool _atomic() const override;
  std::string to_string() const override;
};

class Plus : public Regex {
private:
  std::shared_ptr<Regex> expr;

public:
  explicit Plus(std::shared_ptr<Regex> e);
  std::unique_ptr<NDFA> to_ndfa() const override;
  bool _atomic() const override;
  std::string to_string() const override;
};

} // namespace fa::regex

#endif // !REGEX_HPP
