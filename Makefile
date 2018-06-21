APPS = ui floating widgets easing info reflect paintperf

all: $(APPS)

X11=y
TSLIB=ny
LIBPLANES=n
KPLOT=y
GD=n
IMLIB2=n

.SUFFIXES: .o .cpp

CXXFLAGS = -Wall -g -std=c++11 -I. -pedantic -Ofast -flto -fwhole-program -DNDEBUG
LDLIBS = -flto -fwhole-program

.cpp.o :
	$(CXX) -c $(CXXFLAGS) -o $@ $<

headers = event_loop.h geometry.h input.h screen.h utils.h widget.h \
	x11screen.h color.h window.h ui.h kmsscreen.h animation.h \
	timer.h font.h tools.h chart.h palette.h

common_objs = event_loop.o input.o widget.o screen.o utils.o x11screen.o \
	window.o kmsscreen.o color.o animation.o timer.o font.o \
	tools.o chart.o palette.o geometry.o

ui_objs = main.o $(common_objs)

ifeq ($(KPLOT),y)
CXXFLAGS += -Ikplot -DHAVE_KPLOT
LDLIBS += -Lkplot -lkplot
endif

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

ifeq ($(GD),y)
CXXFLAGS += -DHAVE_LIBGD
LDLIBS += -lgd
endif

ifeq ($(IMLIB2),y)
CXXFLAGS += -DHAVE_IMLIB2
LDLIBS += -lImlib2
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

info_objs = info.o $(common_objs)

info: CXXFLAGS += $(shell pkg-config $(DEPS) --cflags)
info: LDLIBS += $(shell pkg-config $(DEPS) --libs)

$(info_objs): $(headers)

info: $(info_objs)
	$(CXX) $(CXXFLAGS) $(info_objs) -o $@ $(LDLIBS)

reflect_objs = reflect.o $(common_objs)

reflect: CXXFLAGS += $(shell pkg-config $(DEPS) --cflags)
reflect: LDLIBS += $(shell pkg-config $(DEPS) --libs)

$(reflect_objs): $(headers)

reflect: $(reflect_objs)
	$(CXX) $(CXXFLAGS) $(reflect_objs) -o $@ $(LDLIBS)

paintperf_objs = paintperf.o $(common_objs)

paintperf: CXXFLAGS += $(shell pkg-config $(DEPS) --cflags)
paintperf: LDLIBS += $(shell pkg-config $(DEPS) --libs)

$(paintperf_objs): $(headers)

paintperf: $(paintperf_objs)
	$(CXX) $(CXXFLAGS) $(paintperf_objs) -o $@ $(LDLIBS)

clean:
	rm -f $(ui_objs) $(floating_objs) $(widgets_objs) $(easing_objs) \
		$(info_objs) $(reflect_objs) $(paintperf_objs) $(APPS)
