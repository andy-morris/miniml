CXXFLAGS += -std=c++11 -Wall -g

SRCS := $(wildcard *.cxx)
HDRS := $(wildcard *.hxx)

OBJS := $(SRCS:.cxx=.o)

all: miniml

%.o: %.cxx $(wildcard %.hxx)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

main.o: id.hxx
ppr.o:  util.hxx

miniml: main.o $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

clean:
	$(RM) *.o

tags:
	ctags $(SRCS) $(HDRS)

.PHONY: clean
