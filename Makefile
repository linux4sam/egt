all: ui

.SUFFIXES: .o .cpp

CXXFLAGS = -Wall -g -std=c++11 -I. -pedantic

.cpp.o :
	$(CXX) -c $(CXXFLAGS) -o $@ $<

ui_objs = main.o event_loop.o input.o widget.o screen.o utils.o x11screen.o window.o kmsscreen.o

ui: CXXFLAGS += $(shell pkg-config cairo tslib x11 libplanes libdrm libcjson lua --cflags) -DHAVE_TSLIB
ui: LDLIBS += $(shell pkg-config cairo tslib x11 libplanes libdrm libcjson lua --libs)

#ui: CXXFLAGS += $(shell pkg-config cairo tslib x11 --cflags) -DHAVE_TSLIB
#ui: LDLIBS += $(shell pkg-config cairo tslib x11 --libs)

#ui: CXXFLAGS += $(shell pkg-config cairo x11 --cflags)
#ui: LDLIBS += $(shell pkg-config cairo x11 --libs)

$(ui_objs): event_loop.h geometry.h input.h screen.h utils.h widget.h x11screen.h color.h window.h ui.h kmsscreen.h

ui: $(ui_objs)
	$(CXX) $(ui_objs) -o $@ $(LDLIBS)

clean:
	rm -f $(ui_objs) ui
