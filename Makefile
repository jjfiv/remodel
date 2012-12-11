SHELL := /bin/sh
CXX := g++
#CXX := clang++ # uncomment to use clang
PKGS := openssl
CXXFLAGS := -std=c++0x -O2 -Wall -Werror $(shell pkg-config $(PKGS) --cflags)
LDFLAGS := $(shell pkg-config $(PKGS) --libs)

#
# Magic targets:
#

.PHONY: all clean test
all: remodel

clean:
	rm -rf remodel remodel_meta src/*.o

test: all
	python test.py

#
# program object list
#
OBJECTS := src/FileOps.o \
	         src/ProcessManager.o \
					 src/Token.o \
					 src/Parser.o \
					 src/Arguments.o \
					 src/BuildStep.o \
					 src/BuildGraph.o \
					 src/BuildRecord.o \
					 src/TargetBuilder.o \
					 src/main.o

# link executable
remodel: $(OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)


