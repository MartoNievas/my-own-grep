#include "../include/fa/parser/parser.hpp"
#include "../include/fa/regex/regex.hpp"
#include <cctype>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace std;
using namespace fa::regex;

#define BOLD_RED "\033[1;31m"
#define RESET "\033[0m"

static const vector<string> flags = {
    "-c    Print only a count of matching lines per file.",
    "-v    Select non-matching lines (invert match).",
    "-n    Print line number with output lines.",
    "-i    Ignore case distinctions.",
    "-w    Match only whole words.",
    "-x    Match only whole lines.",
    "-h    Display this help text and exit."};

struct Flags {
  bool count = false;        // -c
  bool invert_match = false; // -v
  bool line_number = false;  // -n
  bool ignore_case = false;  // -i
  bool word_regexp = false;  // -w
  bool line_regexp = false;  // -x
  bool help = false;         // -h
};

struct Args {
  Flags flags;
  string regex;
  string filepath;
  bool valid = false;
};

static Args parse_args(int argc, char **argv) {
  Args args;
  vector<string_view> positional;

  for (int i = 1; i < argc; i++) {
    string_view arg(argv[i]);
    if (arg[0] == '-') {
      for (char c : arg.substr(1)) {
        switch (c) {
        case 'c':
          args.flags.count = true;
          break;
        case 'v':
          args.flags.invert_match = true;
          break;
        case 'n':
          args.flags.line_number = true;
          break;
        case 'i':
          args.flags.ignore_case = true;
          break;
        case 'w':
          args.flags.word_regexp = true;
          break;
        case 'x':
          args.flags.line_regexp = true;
          break;
        case 'h':
          args.flags.help = true;
          return args;
        default:
          cerr << format("Unknown flag: -{}\n", c);
          return args;
        }
      }
    } else {
      positional.push_back(arg);
    }
  }

  if (positional.size() != 2) {
    cerr << format("Usage: {} [OPTION]... REGEX FILE\n", argv[0]);
    return args;
  }

  args.regex = string(positional[0]); // copia a string, seguro
  args.filepath = string(positional[1]);
  args.valid = true;
  return args;
}

static string to_lower(string_view s) {
  string result(s);
  for (char &c : result)
    c = tolower((unsigned char)c);
  return result;
}

static bool at_word_boundary(string_view line, size_t start, size_t len) {
  bool left_ok = (start == 0) || !isalnum((unsigned char)line[start - 1]);
  bool right_ok = (start + len >= line.size()) ||
                  !isalnum((unsigned char)line[start + len]);
  return left_ok && right_ok;
}

static string process_line(string_view line, const shared_ptr<Regex> &engine,
                           const Flags &flags, bool &has_match) {
  string output;
  size_t pos = 0;
  has_match = false;

  if (flags.line_regexp) {
    string_view test_view = line;
    string test_str;
    if (flags.ignore_case) {
      test_str = to_lower(line);
      test_view = test_str;
    }
    if (engine->match(test_view)) {
      has_match = true;
      return string(BOLD_RED) + string(line) + RESET;
    }
    return string(line);
  }

  while (pos < line.size()) {
    int longest = -1;
    string_view remaining = line.substr(pos);
    string lower_remaining;
    if (flags.ignore_case)
      lower_remaining = to_lower(remaining);

    for (size_t len = 1; len <= remaining.size(); ++len) {
      string_view candidate = flags.ignore_case
                                  ? string_view(lower_remaining).substr(0, len)
                                  : remaining.substr(0, len);

      if (engine->match(candidate)) {
        if (!flags.word_regexp || at_word_boundary(line, pos, len))
          longest = len;
      } else if (longest != -1) {
        break;
      }
    }

    if (longest != -1) {
      output += BOLD_RED;
      output.append(line.data() + pos, longest);
      output += RESET;
      pos += longest;
      has_match = true;
    } else {
      output += line[pos++];
    }
  }

  return output;
}

static void help_handle() {
  cout << "Usage: ./bin/grep [OPTION]... REGEX [FILE]...\n";
  cout << "Example: ./bin/grep -i 'hello_world' main.c\n\n";
  cout << "Flags supported by program:\n";

  for (const string &flag : flags) {
    cout << "  " << flag << "\n";
  }
}

int main(int argc, char *argv[]) {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  Args args = parse_args(argc, argv);
  if (!args.valid) {
    if (args.flags.help) {
      help_handle();
    }
    return 1;
  }
  try {
    Parser parser(args.regex);
    shared_ptr<Regex> engine = parser.parse();

    ifstream file(args.filepath);
    if (!file.is_open()) {
      cerr << format("Error: cannot open '{}'\n", args.filepath);
      return 1;
    }

    string line, global_buffer;
    global_buffer.reserve(65536);
    int match_count = 0, line_num = 0;

    while (getline(file, line)) {
      line_num++;
      bool has_match = false;
      string output = process_line(line, engine, args.flags, has_match);

      bool print = args.flags.invert_match ? !has_match : has_match;

      if (print) {
        match_count++;
        if (!args.flags.count) {
          if (args.flags.line_number)
            global_buffer += to_string(line_num) + ": ";
          global_buffer += args.flags.invert_match ? line : output;
          global_buffer += '\n';
        }
      }

      if (global_buffer.size() > 32768) {
        cout << global_buffer;
        global_buffer.clear();
      }
    }

    if (args.flags.count)
      global_buffer += to_string(match_count) + '\n';

    cout << global_buffer;

  } catch (const exception &e) {
    cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
