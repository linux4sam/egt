noinst_LTLIBRARIES = libspdlog.la

MRESG_V_GEN = $(mresg_v_GEN_$(V))
mresg_v_GEN_ = $(mresg_v_GEN_$(AM_DEFAULT_VERBOSITY))
mresg_v_GEN_0 = @echo "  GEN     " $@;

libspdlog_la_SOURCES = spdlog/src/spdlog.cpp

libspdlog_la_CXXFLAGS = \
	-I$(top_srcdir)/external/spdlog/include \
	$(AM_CXXFLAGS)

dist-hook:
	for file in $(BUILT_SOURCES) ; do \
		rm -f $(distdir)/$$file ; \
	done
	rm -f $(distdir)/Simd/prj/cmake/CMakeCache.txt

EXTRA_DIST = \
	rapidxml \
	layout \
	utfcpp \
	spdlog \
	googletest \
	Simd \
	spdlog \
	cxxopts

if HAVE_SIMD
all-local:
	$(MAKE) -C Simd/prj/cmake/ $(AM_MAKEFLAGS) all

clean-local:
	$(MAKE) -C Simd/prj/cmake/ $(AM_MAKEFLAGS) clean
endif
