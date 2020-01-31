if ENABLE_LUA_BINDINGS

SWIG_SRC = ui.i
SWIG_FLAGS = -c++ -lua -DEGT_API

SWIG_V_GEN = $(swig_v_GEN_$(V))
swig_v_GEN_ = $(swig_v_GEN_$(AM_DEFAULT_VERBOSITY))
swig_v_GEN_0 = @echo "  SWIG    " $@;

ui.cpp: ui.i
if AMDEP
	@$(SWIG_V_GEN)$(SWIG_BIN) $(SWIG_FLAGS) -I$(top_srcdir)/include -MT $@ -MM $< > $(DEPDIR)/ui.i.dep.tmp
	@mv $(DEPDIR)/ui.i.dep.tmp $(DEPDIR)/ui.i.dep
endif
	$(SWIG_V_GEN)$(SWIG_BIN) $(SWIG_FLAGS) -I$(top_srcdir)/include -o $@ $<
	sed -i 's/enum //g' ui.cpp # fix generated swig code

@AMDEP_TRUE@@am__include@ @am__quote@./$(DEPDIR)/ui.i.dep@am__quote@

CLEANFILES = ui.cpp

CUSTOM_CXXFLAGS = -Woverloaded-virtual -DLUA_USE_POSIX

noinst_LTLIBRARIES = libegtlua.la
libegtlua_la_SOURCES =
nodist_libegtlua_la_SOURCES = ui.cpp
libegtlua_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	$(CUSTOM_CXXFLAGS) \
	@LIBEGT_EXTRA_CXXFLAGS@ \
	$(CODE_COVERAGE_CXXFLAGS) \
	$(AM_CXXFLAGS)

endif

BUILT_SOURCES = ui.cpp
EXTRA_DIST = ui.i
