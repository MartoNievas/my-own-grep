CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -I./include
SRCDIR = src
TESTDIR = test
BINDIR = bin

AUTOMATA_SRC = $(SRCDIR)/automata/dfa.cpp $(SRCDIR)/automata/ndfa.cpp
REGEX_SRC = $(SRCDIR)/regex/regex.cpp

TEST_SRC_BASIC_METHODS = $(TESTDIR)/test_basic_methods.cpp
TEST_SRC_DETERMINIZE = $(TESTDIR)/test_determinize.cpp
TEST_SRC_MINIMIZE = $(TESTDIR)/test_minimize.cpp
TEST_SRC_REGEX = $(TESTDIR)/test_regex.cpp

TEST_BIN_BASIC = $(BINDIR)/test_basic_methods
TEST_BIN_DETERMINIZE = $(BINDIR)/test_determinize
TEST_BIN_MINIMIZE = $(BINDIR)/test_minimize
TEST_BIN_REGEX = $(BINDIR)/test_regex

ALL_TESTS = $(TEST_BIN_BASIC) $(TEST_BIN_DETERMINIZE) $(TEST_BIN_MINIMIZE) $(TEST_BIN_REGEX)

.PHONY: all clean test test_basic test_determinize test_minimize test_regex test_all

all: $(ALL_TESTS)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TEST_BIN_BASIC): $(AUTOMATA_SRC) $(TEST_SRC_BASIC_METHODS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BIN_DETERMINIZE): $(AUTOMATA_SRC) $(TEST_SRC_DETERMINIZE) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BIN_MINIMIZE): $(AUTOMATA_SRC) $(TEST_SRC_MINIMIZE) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BIN_REGEX): $(AUTOMATA_SRC) $(REGEX_SRC) $(TEST_SRC_REGEX) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

test_basic: $(TEST_BIN_BASIC)
	./$(TEST_BIN_BASIC)

test_determinize: $(TEST_BIN_DETERMINIZE)
	./$(TEST_BIN_DETERMINIZE)

test_minimize: $(TEST_BIN_MINIMIZE)
	./$(TEST_BIN_MINIMIZE)

test_regex: $(TEST_BIN_REGEX)
	./$(TEST_BIN_REGEX)

test_all: $(ALL_TESTS)
	@echo "Running basic methods tests..."
	./$(TEST_BIN_BASIC)
	@echo "\nRunning determinization tests..."
	./$(TEST_BIN_DETERMINIZE)
	@echo "\nRunning minimization tests..."
	./$(TEST_BIN_MINIMIZE)
	@echo "\nRunning regex tests..."
	./$(TEST_BIN_REGEX)

test: test_all

clean:
	rm -rf $(BINDIR)
