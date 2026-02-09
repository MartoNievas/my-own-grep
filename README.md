# Grep-Style Regular Expression Engine with Finite Automata

## About
This project is my own implementation of a **grep-based regular expression engine** built from scratch. It explores how regex engines work by constructing a lexer, parser, and finite automata (NDFA/DFA) to perform pattern matching similar to traditional tools like `grep`. The goal is to learn and experiment, so some components are still under development.

## What This Engine Does
At a high level, the engine takes a regular expression, processes it through several stages, and uses finite automata to match patterns in text:
- **Lexer**: Tokenizes the regex pattern (e.g., symbols, operators like `*`, `+`, `|`, parentheses).
- **Parser**: Builds a structured representation of the regex from the token stream.
- **Finite Automata**: Converts the parsed regex into an NDFA and/or DFA for efficient pattern matching.

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
