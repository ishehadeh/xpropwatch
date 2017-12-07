CCC=g++
LIBS=-lxcb
CFALGS=$(LIBS) -g -std=c++17 -I./src

all: xpropwatch

%.o: src/%.cpp
	$(CCC) $(CFLAGS) -c -o $@ $?

xpropwatch: core.o xpropwatch.o
	$(CCC) $(CFALGS) -o $@ $?

clean:
	rm -f xpropwatch
	rm -f *.o

.PHONY: clean
