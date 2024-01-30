CXX = g++
CXXFLAGS = -std=c++17 $(shell pkg-config --cflags libegt) \
    -I../../src/detail/ -I../../external/cxxopts/include/ \
    -I../../external/rapidxml/ -Wall

LDFLAGS = $(shell pkg-config --libs libegt)

all: gfx-convert

gfx-convert: gfx-convert.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f gfx-convert eraw.*
