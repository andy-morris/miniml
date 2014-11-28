INCLUDES += -Iinclude -Ibuild
WARNS    += -Wall -Wdocumentation
CXXFLAGS += -std=c++11 $(WARNS) -g $(INCLUDES)
CFLAGS   += $(WARNS) -g $(INCLUDES)
RAGELFLAGS += -G2

SRCS := $(shell find src -name '*.cxx') build/lexer.cxx build/lemon.cxx
HDRS := $(shell find include -name '*.hxx') build/lemon.hxx
OBJS := $(patsubst src/%,build/%,$(patsubst %.cxx,%.o,$(SRCS)))
DEPS := $(OBJS:.o=.d)

all: miniml

build/%.o: src/%.cxx
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
build/%.o: build/%.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Ragel
build/%.cxx: src/%.cxx.rl
	@mkdir -p $(dir $@)
	ragel $(RAGELFLAGS) -o $@ $<

# Lemon
build/%.cxx build/%.hxx %.out: src/%.y
	@mkdir -p $(dir $@)
	lemon $<
	mv src/$*.out .
	mv src/$*.c build/$*.cxx
	mv src/$*.h build/$*.hxx


doc: Doxyfile $(SRCS) $(HDRS)
	doxygen

miniml: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

tags: $(SRCS) $(HDRS)
	ctags $^

clean:
	$(RM) -r build doc tags miniml *.out
.PHONY: clean

## Dependencies
Makefile: $(DEPS)
build/%.d: src/%.cxx
	@mkdir -p $(dir $@)
	$(CXX) $(INCLUDES) -MM $< -MF $@ -MT build/$(basename $*).o
build/%.d: build/%.cxx
	$(CXX) $(INCLUDES) -MM $< -MF $@ -MT build/$(basename $*).o
-include $(DEPS)
