INCLUDES += -Iinclude -Ibuild
WARNS    += -Wall -Wdocumentation
CXXFLAGS += -std=c++1y $(WARNS) -g $(INCLUDES)
RAGELFLAGS += -G2

SRCS := $(wildcard src/*.cxx)     build/lexer.cxx
HDRS := $(wildcard include/*.hxx)
OBJS := $(patsubst src/%,build/%,$(patsubst %.cxx,%.o,$(SRCS)))
DEPS := $(patsubst src/%,build/%,$(patsubst %.cxx,%.d,$(SRCS)))

all: miniml

build/%.o: src/%.cxx
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c -o $@ $<
build/%.o: build/%.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Ragel
build/%.cxx: src/%.cxx.rl
	@mkdir -p build
	ragel $(RAGELFLAGS) -o $@ $<


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
	$(CXX) $(INCLUDES) -MM $< -MF $@ -MT build/$(basename $*).o
build/%.d: build/%.cxx
	$(CXX) $(INCLUDES) -MM $< -MF $@ -MT build/$(basename $*).o
-include $(DEPS)
