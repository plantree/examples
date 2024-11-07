CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror

SRCS := $(wildcard *_test.cc)
TARGETS := $(SRCS:.cc=)

all: $(TARGETS)

%_test: %_test.cc
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
