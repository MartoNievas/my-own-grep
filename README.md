# Grep-Style Regular Expression Engine with Finite Automata

## About
This project is my own implementation of a **grep-based regular expression engine** built from scratch. It explores how regex engines work by constructing a lexer, parser, and finite automata (NDFA/DFA) to perform pattern matching similar to traditional tools like `grep`. The goal is to learn and experiment, so some components are still under development.

## What This Engine Does
At a high level, the engine takes a regular expression, processes it through several stages, and uses finite automata to match patterns in text:
- **Lexer**: Tokenizes the regex pattern (e.g., symbols, operators like `*`, `+`, `|`, parentheses).
- **Parser**: Builds a structured representation of the regex from the token stream.
- **Finite Automata**: Converts the parsed regex into an NDFA and/or DFA for efficient pattern matching.

## Usage
```bash
./grep [OPTION]... REGEX FILE
```

**Examples:**
```bash
./grep "a"          text.txt   # lines containing 'a'
./grep "ab"         text.txt   # lines containing 'ab'
./grep "a|b"        text.txt   # lines containing 'a' or 'b'
./grep "ab*"        text.txt   # lines containing 'a' followed by zero or more 'b'
./grep "ab+"        text.txt   # lines containing 'a' followed by one or more 'b'
./grep "(ab|cd)+"   text.txt   # lines containing one or more repetitions of 'ab' or 'cd'
./grep -n "a"       text.txt   # show line numbers
./grep -v "a"       text.txt   # lines that do NOT contain 'a'
./grep -c "a"       text.txt   # count matching lines
./grep -in "a"      text.txt   # flags can be combined
```

## Supported Operations

| Operation     | Syntax   | Description                                   |
|---------------|----------|-----------------------------------------------|
| Literal       | `a`      | Matches the exact character `a`               |
| Concatenation | `ab`     | Matches `a` followed by `b`                   |
| Union         | `a\|b`   | Matches either `a` or `b`                     |
| Kleene Star   | `a*`     | Matches zero or more repetitions of `a`       |
| Plus          | `a+`     | Matches one or more repetitions of `a`        |
| Grouping      | `(ab)`   | Groups expressions to apply operators to them |
| Empty         | `∅`      | Matches nothing                               |
| Lambda        | `λ`      | Matches the empty string                      |

Operations can be combined and nested using parentheses:
- `(a|b)*` — any string of `a`s and `b`s, including empty
- `(ab)+` — one or more repetitions of `ab`
- `a(b|c)*d` — `a`, then any combination of `b` and `c`, then `d`

## Supported Flags

Flags can be placed anywhere in the command and combined together (e.g. `-in`, `-vn`).

| Flag | Description                                                  |
|------|--------------------------------------------------------------|
| `-c` | Print only the count of matching lines                       |
| `-v` | Invert match — print lines that do **not** match            |
| `-n` | Prefix each output line with its line number                 |
| `-i` | Ignore case — match regardless of upper/lowercase            |
| `-w` | Word match — only match if pattern is at a word boundary     |
| `-x` | Line match — only match if the entire line matches the regex |

**Examples:**
```bash
./grep -n  "ab"   text.txt   # show line numbers
./grep -v  "ab"   text.txt   # lines that do NOT contain 'ab'
./grep -c  "ab"   text.txt   # count of matching lines
./grep -i  "AB"   text.txt   # case-insensitive match
./grep -w  "ab"   text.txt   # 'ab' only as a whole word
./grep -x  "ab"   text.txt   # lines where the entire content is 'ab'
./grep -in "AB"   text.txt   # combined: case-insensitive + line numbers
./grep -vn "ab"   text.txt   # combined: invert match + line numbers
```

## Key Concepts
- **Regular Expression Engines**  
  Modern regex implementations (e.g., those in languages and tools) vary in design, but many rely on automata theory for pattern matching performance and correctness.
  
- **Lexer and Parser**  
  The lexer breaks the raw pattern into tokens; the parser organizes those tokens according to a grammar to form a syntax structure that can be translated into automata.

- **Finite Automata**
  - **NDFA (Nondeterministic Finite Automaton)**: Easy to construct from regex syntax, can have multiple possible transitions.
  - **DFA (Deterministic Finite Automaton)**: Derived from an NDFA, has a single transition per input per state, enabling faster execution.

- **grep**  
  The Unix tool `grep` (Global Regular Expression Print) is a classic example of a regex engine used for searching text with patterns. This project is inspired by the matching and automata-based techniques commonly associated with `grep`.

## References
- Turing machine — https://en.wikipedia.org/wiki/Turing_machine  
- Finite-state machine — https://en-wikipedia-org.translate.goog/wiki/Finite-state_machine?_x_tr_sl=en&_x_tr_tl=es&_x_tr_hl=es&_x_tr_pto=tc

## Status
This is a work-in-progress implementation of the core components of a regex engine. Contributions, suggestions, and improvements are welcome!
