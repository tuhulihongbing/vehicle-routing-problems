
CPP = g++
UTIL = ../baseClasses
CPPFLAGS = -g -O0 -MMD  -MP  -I$(UTIL)
LDFLAGS = -lgd -I$(UTIL) -L$(UTIL)


SRCS = $(wildcard *.cpp) 
OBJS = $(SRCS:.cpp=.o)
UTILSRCS = $(wildcard $(UTIL)/*.cpp)
UTILOBJS = $(UTILSRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)


all: $(OBJS)

$(UTIL)/%.o: $(UTIL)/%.cpp
	$(MAKE) -C $(UTIL)

tester: $(UTILOBJS) $(OBJS)
	$(CPP) $^ -o $@ $(LDFLAGS)

%.o: %.cpp ../baseClasses/twpath.h
	$(CPP) $(CPPFLAGS) -c $< -o $@

test: tester lc008.txt
	./tester lc008.txt

valgrind: tester lc008.txt 
	valgrind -v --track-origins=yes --leak-check=full ./tester p50.txt

.PHONY: clean

clean:
	rm -f tester $(SRCS:.cpp=.o) $(DEPS) out/*.png ./*.png
	$(MAKE) -C $(UTIL) clean

-include $(DEPS)

