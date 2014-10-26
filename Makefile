CXXFLAGS += -std=c++1y -Wall -g -Iinclude

SRCS := $(wildcard src/*.cxx)
HDRS := $(wildcard include/*.hxx)
OBJS := $(patsubst src/%.cxx,build/%.o,$(SRCS))
DEPS := $(patsubst src/%.cxx,build/%.d,$(SRCS))

all: miniml

build/%.o: src/%.cxx
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

doc: Doxyfile $(SRCS) $(HDRS)
	doxygen

miniml: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

tags: $(SRCS) $(HDRS)
	ctags $^

clean:
	$(RM) -r build doc tags miniml
.PHONY: clean

## Dependencies
Makefile: $(DEPS)
build/%.d: src/%.cxx
	@mkdir -p build
	$(CXX) -Iinclude -MM $< -o $@
-include $(DEPS)
