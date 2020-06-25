CUSTOM_CXXFLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-isystem $(top_srcdir)/external/rapidxml \
	$(cairo_CFLAGS) \
	$(CODE_COVERAGE_CXXFLAGS)

CUSTOM_LDADD = $(cairo_LIBS) $(CODE_COVERAGE_LDFLAGS)

bin_PROGRAMS = snippets snippet0 snippet1 snippet2 \
	snippet3 snippet4 snippet5 snippet6 snippet7 \
	snippet8 snippet9

if HAVE_PLPLOT
bin_PROGRAMS += \
snippet10
endif

snippets_SOURCES = snippets.cpp
nodist_snippets_SOURCES = $(BUILT_SOURCES)
snippets_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
snippets_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
snippetsdir = $(prefix)/share/egt/examples/snippets
snippets_DATA = $(wildcard $(top_srcdir)/examples/snippets/assets/*.png) \
	$(wildcard $(top_srcdir)/examples/snippets/assets/*.wav) \
	$(wildcard $(top_srcdir)/examples/snippets/assets/*.mp3)
snippets_LDFLAGS = $(AM_LDFLAGS)

snippet0_SOURCES = snippet0.cpp
snippet0_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
snippet0_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)

snippet1_SOURCES = snippet1.cpp
snippet1_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
snippet1_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)

snippet2_SOURCES = snippet2.cpp
snippet3_SOURCES = snippet3.cpp
snippet4_SOURCES = snippet4.cpp

snippet5_SOURCES = snippet5.cpp
snippet5_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
snippet5_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
snippet5_LDFLAGS = -Wl,--no-as-needed $(AM_LDFLAGS)

snippet6_SOURCES = snippet6.cpp
snippet6_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
snippet6_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
snippet6_LDFLAGS = -Wl,--no-as-needed $(AM_LDFLAGS)

snippet7_SOURCES = snippet7.cpp
snippet8_SOURCES = snippet8.cpp
snippet9_SOURCES = snippet9.cpp

snippet10_SOURCES = snippet10.cpp
snippet10_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
snippet10_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)

tidy:
