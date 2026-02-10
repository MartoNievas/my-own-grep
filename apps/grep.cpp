#include "../include/fa/parser/parser.hpp"
#include "../include/fa/regex/regex.hpp"
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace fa::regex;

#define BOLD_RED "\033[1;31m"
#define RESET "\033[0m"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << format("Usage: {} <Regex> <filepath>", argv[0]) << endl;
    return 1;
  }

  string regex_str = argv[1];
  string filepath = argv[2];

  try {
    Parser parser(regex_str);
    auto regex_engine = parser.parse();

    ifstream file(filepath);
    if (!file.is_open()) {
      cerr << "Error: Could not open the file '" << filepath << "'" << endl;
      return 1;
    }

    string line;
    while (getline(file, line)) {
      stringstream ss(line);
      string word;
      vector<string> words;
      bool line_has_match = false;

      while (ss >> word) {
        if (regex_engine->match(word)) {
          words.push_back(string(BOLD_RED) + word + RESET);
          line_has_match = true;
        } else {
          words.push_back(word);
        }
      }

      if (line_has_match) {
        for (size_t i = 0; i < words.size(); i++) {
          cout << words[i] << (i == words.size() - 1 ? "" : " ");
        }
        cout << endl;
      }
    }

    file.close();

  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}
