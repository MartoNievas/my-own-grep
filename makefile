CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -I./include
SRCDIR = src
TESTDIR = test
BINDIR = bin
APPDIR = apps

# Fuentes del Motor
AUTOMATA_SRC = $(SRCDIR)/automata/dfa.cpp $(SRCDIR)/automata/ndfa.cpp
REGEX_SRC    = $(SRCDIR)/regex/regex.cpp
LEXER_SRC    = $(SRCDIR)/lexer/lexer.cpp $(SRCDIR)/lexer/token.cpp
PARSER_SRC   = $(SRCDIR)/parser/parser.cpp 

# Fuente de la Aplicación
GREP_SRC     = $(APPDIR)/grep.cpp
GREP_BIN_APP = $(BINDIR)/grep

# Fuentes de Tests
TEST_SRC_BASIC_METHODS = $(TESTDIR)/automata/test_basic_methods.cpp
TEST_SRC_DETERMINIZE    = $(TESTDIR)/automata/test_determinize.cpp
TEST_SRC_MINIMIZE       = $(TESTDIR)/automata/test_minimize.cpp
TEST_SRC_REGEX          = $(TESTDIR)/regex/test_regex.cpp
TEST_SRC_LEXER          = $(TESTDIR)/lexer/test_lexer.cpp
TEST_SRC_PARSER         = $(TESTDIR)/parser/test_parser.cpp

# Binarios de Tests
TEST_BIN_BASIC       = $(BINDIR)/test_basic_methods
TEST_BIN_DETERMINIZE = $(BINDIR)/test_determinize
TEST_BIN_MINIMIZE    = $(BINDIR)/test_minimize
TEST_BIN_REGEX       = $(BINDIR)/test_regex
TEST_BIN_LEXER       = $(BINDIR)/test_lexer
TEST_BIN_PARSER      = $(BINDIR)/test_parser

ALL_TESTS = $(TEST_BIN_BASIC) $(TEST_BIN_DETERMINIZE) $(TEST_BIN_MINIMIZE) $(TEST_BIN_REGEX) $(TEST_BIN_LEXER) $(TEST_BIN_PARSER)

.PHONY: all clean test test_all grep

# 'all' ahora construye la aplicación grep por defecto
all: $(BINDIR) $(GREP_BIN_APP)

$(BINDIR):
	mkdir -p $(BINDIR)

# --- Regla de la Aplicación ---
$(GREP_BIN_APP): $(AUTOMATA_SRC) $(LEXER_SRC) $(REGEX_SRC) $(PARSER_SRC) $(GREP_SRC) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# --- Reglas de Tests ---
$(TEST_BIN_BASIC): $(AUTOMATA_SRC) $(TEST_SRC_BASIC_METHODS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BIN_DETERMINIZE): $(AUTOMATA_SRC) $(TEST_SRC_DETERMINIZE) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BIN_MINIMIZE): $(AUTOMATA_SRC) $(TEST_SRC_MINIMIZE) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BIN_REGEX): $(AUTOMATA_SRC) $(REGEX_SRC) $(TEST_SRC_REGEX) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BIN_LEXER): $(LEXER_SRC) $(TEST_SRC_LEXER) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BIN_PARSER): $(AUTOMATA_SRC) $(LEXER_SRC) $(REGEX_SRC) $(PARSER_SRC) $(TEST_SRC_PARSER) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# --- Comandos rápidos ---
grep: $(GREP_BIN_APP)

test_all: $(ALL_TESTS)
	@echo "Ejecutando tests..."
	./$(TEST_BIN_BASIC)
	./$(TEST_BIN_DETERMINIZE)
	./$(TEST_BIN_MINIMIZE)
	./$(TEST_BIN_REGEX)
	./$(TEST_BIN_LEXER)
	./$(TEST_BIN_PARSER)

clean:
	rm -rf $(BINDIR)
