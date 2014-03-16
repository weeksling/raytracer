BINARY=raytracer
SOURCES := $(wildcard *.cpp)
SOURCES += $(wildcard yaml/*.cpp)
OBJECTS := $(SOURCES:.cpp=.o)
CPP=g++
CPPFLAGS=-O0
LDFLAGS=

all: $(SOURCES) $(BINARY)

$(BINARY): $(OBJECTS) 
	$(CPP) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CPP) -g -c $(CPPFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(BINARY)
