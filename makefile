CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -I./include
SRCDIR = src
TESTDIR = test
BINDIR = bin

# Archivos fuente
AUTOMATA_SRC = $(SRCDIR)/automata/dfa.cpp $(SRCDIR)/automata/ndfa.cpp
TEST_SRC = $(TESTDIR)/test.cpp

# Ejecutables
TEST_BIN = $(BINDIR)/test

.PHONY: all clean test

all: $(TEST_BIN)

$(BINDIR):
	mkdir -p $(BINDIR)

$(TEST_BIN): $(AUTOMATA_SRC) $(TEST_SRC) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BINDIR)
