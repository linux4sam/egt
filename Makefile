all: ui floating widgets easing

X11=n
TSLIB=y
LIBPLANES=y

.SUFFIXES: .o .cpp

CXXFLAGS = -Wall -g -std=c++11 -I. -pedantic -Ofast

.cpp.o :
	$(CXX) -c $(CXXFLAGS) -o $@ $<

headers = event_loop.h geometry.h input.h screen.h utils.h widget.h \
	x11screen.h color.h window.h ui.h kmsscreen.h animation.h piechart.h \
	timer.h font.h

common_objs = event_loop.o input.o widget.o screen.o utils.o x11screen.o \
	window.o kmsscreen.o color.o animation.o piechart.o timer.o font.o

ui_objs = main.o $(common_objs)

DEPS = cairo
ifeq ($(X11),y)
DEPS += x11
CXXFLAGS += -DHAVE_X11
endif

ifeq ($(TSLIB),y)
DEPS += tslib
CXXFLAGS += -DHAVE_TSLIB
endif

ifeq ($(LIBPLANES),y)
CXXFLAGS += -DHAVE_LIBPLANES
DEPS += libdrm libcjson lua libplanes
endif

ui: CXXFLAGS += $(shell pkg-config $(DEPS) --cflags)
ui: LDLIBS += $(shell pkg-config $(DEPS) --libs)

$(ui_objs): $(headers)

ui: $(ui_objs)
	$(CXX) $(CXXFLAGS) $(ui_objs) -o $@ $(LDLIBS)

floating_objs = floating.o $(common_objs)

floating: CXXFLAGS += $(shell pkg-config $(DEPS) --cflags)
floating: LDLIBS += $(shell pkg-config $(DEPS) --libs)

$(floating_objs): $(headers)

floating: $(floating_objs)
	$(CXX) $(CXXFLAGS) $(floating_objs) -o $@ $(LDLIBS)

widgets_objs = widgets.o $(common_objs)

widgets: CXXFLAGS += $(shell pkg-config $(DEPS) --cflags)
widgets: LDLIBS += $(shell pkg-config $(DEPS) --libs)

$(widgets_objs): $(headers)

widgets: $(widgets_objs)
	$(CXX) $(CXXFLAGS) $(widgets_objs) -o $@ $(LDLIBS)

easing_objs = easing.o $(common_objs)

easing: CXXFLAGS += $(shell pkg-config $(DEPS) --cflags)
easing: LDLIBS += $(shell pkg-config $(DEPS) --libs)

$(easing_objs): $(headers)

easing: $(easing_objs)
	$(CXX) $(CXXFLAGS) $(easing_objs) -o $@ $(LDLIBS)

clean:
	rm -f $(ui_objs) $(floating_objs) $(widgets_objs) $(easing_objs) ui floating widgets easing
