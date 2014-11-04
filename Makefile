INCLUDES += -Iinclude -Ibuild
WARNS    += -Wall -Wdocumentation
CXXFLAGS += -std=c++1y $(WARNS) -g $(INCLUDES)
CFLAGS   += $(WARNS) -g $(INCLUDES)
RAGELFLAGS += -G2

SRCS := $(wildcard src/*.cxx) build/lexer.cxx build/lemon.cxx
HDRS := $(wildcard include/*.hxx) build/lemon.hxx
OBJS := $(addprefix build/,$(addsuffix .o,$(basename $(notdir $(SRCS)))))
DEPS := $(addprefix build/,$(addsuffix .d,$(basename $(notdir $(SRCS)))))

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

# Lemon
build/%.cxx build/%.hxx %.out: src/%.y
	@mkdir -p build
	lemon -l $<
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
	@mkdir -p build
	$(CXX) $(INCLUDES) -MM $< -MF $@ -MT build/$(basename $*).o
build/%.d: build/%.cxx
	$(CXX) $(INCLUDES) -MM $< -MF $@ -MT build/$(basename $*).o
-include $(DEPS)
