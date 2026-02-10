#include "../include/fa/parser/parser.hpp"
#include "../include/fa/regex/regex.hpp"
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

#define BOLD_RED "\033[1;31m"
#define RESET "\033[0m"

int main(int argc, char *argv[]) {
  // OPTIMIZACIÓN 1: I/O Ultra rápido
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  if (argc != 3) {
    cerr << format("Uso: {} <Regex> <archivo>", argv[0]) << endl;
    return 1;
  }

  string regex_str = argv[1];
  string filepath = argv[2];

  try {
    Parser parser(regex_str);
    auto regex_engine = parser.parse();

    ifstream file(filepath);
    if (!file.is_open())
      return 1;

    string line;
    string global_buffer;
    global_buffer.reserve(65536);

    while (getline(file, line)) {
      bool line_has_match = false;
      size_t current_pos = 0;
      string line_output = "";

      while (current_pos < line.length()) {
        int longest_match = -1;
        string_view remaining_view(line.data() + current_pos,
                                   line.length() - current_pos);

        for (size_t len = 1; len <= remaining_view.length(); ++len) {
          if (regex_engine->match(string(remaining_view.substr(0, len)))) {
            longest_match = len;
          } else if (longest_match != -1) {
            break;
          }
        }

        if (longest_match != -1) {
          line_output += BOLD_RED;
          line_output += line.substr(current_pos, longest_match);
          line_output += RESET;
          current_pos += longest_match;
          line_has_match = true;
        } else {
          line_output += line[current_pos];
          current_pos++;
        }
      }

      if (line_has_match) {
        global_buffer += line_output;
        global_buffer += '\n';
      }

      if (global_buffer.size() > 32768) {
        cout << global_buffer;
        global_buffer.clear();
      }
    }

    cout << global_buffer;
    file.close();

  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}
