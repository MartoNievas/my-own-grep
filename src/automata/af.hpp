#ifndef AF_HPP
#define AF_HPP

#include <map>
#include <set>
#include <string>

class AF {
protected:
  std::set<std::string> states;
  std::string initial_state;
  std::map<std::string, std::map<std::string, std::set<std::string>>>
      transitions;
  std::set<std::string> final_states;
  std::set<std::string> alphabet;

public:
  AF();
  virtual ~AF() = default;

  void add_state(const std::string &state, bool final = false);
  void mark_initial_state(const std::string &state);
  int size() const;
  AF &normalize_states(void);

protected:
  void _rename_state(const std::string &, const std::string &);
};

#endif
