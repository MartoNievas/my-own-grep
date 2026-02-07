CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -I./include
SRCDIR = src
TESTDIR = test
BINDIR = bin

# Archivos fuente
AUTOMATA_SRC = $(SRCDIR)/automata/dfa.cpp $(SRCDIR)/automata/ndfa.cpp

# Archivos de test
TEST_SRC_BASIC_METHODS = $(TESTDIR)/test_basic_methods.cpp
TEST_SRC_DETERMINIZE = $(TESTDIR)/test_determinize.cpp

# Ejecutables
TEST_BIN_BASIC = $(BINDIR)/test_basic_methods
TEST_BIN_DETERMINIZE = $(BINDIR)/test_determinize

# Todos los tests
ALL_TESTS = $(TEST_BIN_BASIC) $(TEST_BIN_DETERMINIZE)

.PHONY: all clean test test_basic test_determinize test_all

all: $(ALL_TESTS)

$(BINDIR):
	mkdir -p $(BINDIR)

# Compilar test básico
$(TEST_BIN_BASIC): $(AUTOMATA_SRC) $(TEST_SRC_BASIC_METHODS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compilar test de determinización
$(TEST_BIN_DETERMINIZE): $(AUTOMATA_SRC) $(TEST_SRC_DETERMINIZE) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Ejecutar solo test básico
test_basic: $(TEST_BIN_BASIC)
	./$(TEST_BIN_BASIC)

# Ejecutar solo test de determinización
test_determinize: $(TEST_BIN_DETERMINIZE)
	./$(TEST_BIN_DETERMINIZE)

# Ejecutar todos los tests
test_all: $(ALL_TESTS)
	@echo "Running basic methods tests..."
	./$(TEST_BIN_BASIC)
	@echo "\nRunning determinization tests..."
	./$(TEST_BIN_DETERMINIZE)

# Alias para ejecutar todos los tests
test: test_all

clean:
	rm -rf $(BINDIR)
