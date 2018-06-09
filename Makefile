all: ui

.SUFFIXES: .o .cpp

CXXFLAGS = -Wall -g -std=c++11 -I. -Os

.cpp.o :
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ui_objs = main.o event_loop.o input.o widget.o screen.o utils.o

ui: CXXFLAGS += $(shell pkg-config cairo tslib --cflags)
ui: LDLIBS += $(shell pkg-config cairo tslib --libs)

$(ui_objs): event_loop.h geometry.h input.h screen.h utils.h widget.h

ui: $(ui_objs)
	$(CXX) $(ui_objs) -o $@ $(LDLIBS)

clean:
	rm -f $(ui_objs) ui
