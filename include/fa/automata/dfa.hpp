#ifndef DFA_HPP
#define DFA_HPP

#include "fa.hpp"
#include <memory>
#include <string>
class DFA : public FA<std::string> {
public:
  DFA() : FA<std::string>() {}

  std::unique_ptr<DFA> minimize(void);
};

#endif // !DFA_HPP
