dist-hook:
	for file in $(BUILT_SOURCES) ; do \
		rm -f $(distdir)/$$file ; \
	done
	rm -f $(distdir)/Simd/prj/cmake/CMakeCache.txt

EXTRA_DIST = \
	rapidxml \
	layout \
	utfcpp \
	fmt \
	googletest \
	Simd \
	cxxopts

if HAVE_SIMD
all-local:
	$(MAKE) -C Simd/prj/cmake/ $(AM_MAKEFLAGS) all

clean-local:
	$(MAKE) -C Simd/prj/cmake/ $(AM_MAKEFLAGS) clean
endif
