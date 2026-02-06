#ifndef DFA_HPP
#define DFA_HPP

#include "fa.hpp"
#include <string>

class DFA : public FA<std::string> {
public:
  DFA() : FA<std::string>() {}

  DFA *minimize(void);
};

#endif // !DFA_HPP
