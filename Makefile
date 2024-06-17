# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -O3 -Iinclude -Igoogletest/googletest/include -Igoogletest/googletest

# Directories
SRCDIR = src
TESTDIR = tests
BUILDDIR = build
GTESTDIR = googletest/googletest

# Files
TESTFILES = $(wildcard $(TESTDIR)/*.cpp)
TESTOBJFILES = $(patsubst $(TESTDIR)/%.cpp,$(BUILDDIR)/%.o,$(TESTFILES))
GTEST_SRCS_ = $(GTESTDIR)/src/gtest-all.cc
GTEST_HEADERS = $(GTESTDIR)/include/gtest/*.h $(GTESTDIR)/include/gtest/internal/*.h

# Targets
TEST_TARGET = $(BUILDDIR)/runTests

# Rules
all: test

$(BUILDDIR)/%.o: $(TESTDIR)/%.cpp
	mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/gtest-all.o: $(GTEST_SRCS_)
	mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TEST_TARGET): $(TESTOBJFILES) $(BUILDDIR)/gtest-all.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(BUILDDIR)

.PHONY: all test clean
