CXXFLAGS = -std=c++17 $(shell pkg-config --cflags cairo) -Wall -O3 -g \
	 -I../../src/detail/ -I../../include/ -I../../external/cxxopts/include/
LDFLAGS = $(shell pkg-config --libs cairo)

all: eraw-convert

eraw-convert: eraw-convert.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f eraw-convert
