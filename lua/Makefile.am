if ENABLE_LUA_BINDINGS

SWIG_SRC = ui.i

SWIG_V_GEN = $(swig_v_GEN_$(V))
swig_v_GEN_ = $(swig_v_GEN_$(AM_DEFAULT_VERBOSITY))
swig_v_GEN_0 = @echo "  SWIG    " $@;

ui.cpp: ui.i
	$(SWIG_V_GEN)$(SWIG_BIN) -c++ -lua -I$(top_srcdir)/include -o $@ $<
	sed -i 's/enum //g' ui.cpp # fix generated swig code

CLEANFILES = ui.cpp
BUILT_SOURCES = ui.cpp
EXTRA_DIST = ui.i examples

endif
