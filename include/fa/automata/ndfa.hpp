#ifndef NDFA_HPP
#define NDFA_HPP

#include "dfa.hpp"
#include "fa.hpp"
#include <memory>
#include <set>
#include <string>

class NDFA : public FA<std::set<std::string>> {

public:
  NDFA() : FA<std::set<std::string>>() {}

  std::unique_ptr<DFA> determinize(void);

protected:
  std::set<std::string>
  epsilon_clousure(const std::set<std::string> &states) const;

  std::set<std::string> epsilon_clousure(const std::string &state) const;

  std::set<std::string> move(const std::set<std::string> &states,
                             char symbol) const;
};

#endif // !NDFA_HPP
