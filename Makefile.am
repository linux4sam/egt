ACLOCAL_AMFLAGS = -I m4
SUBDIRS = external
if ENABLE_LUA_BINDINGS
SUBDIRS += lua
endif
SUBDIRS += src scripts docs
if ENABLE_EXAMPLES
SUBDIRS += examples
endif
SUBDIRS += test

EXTRA_DIST = README.md \
CONTRIBUTING.md \
CODE_OF_CONDUCT.md \
COPYING

.PHONY: docs docset
docs docset:
if HAVE_DOXYGEN_BIN
	@cd docs && $(MAKE) $(AM_MAKEFLAGS) $@
else
	@echo "configured without doxygen"
endif

ASTYLE_WILDCARDS = \
"$(top_srcdir)/examples/\*.cpp" \
"$(top_srcdir)/include/egt/\*.h" \
"$(top_srcdir)/src/\*.c" \
"$(top_srcdir)/src/\*.cpp" \
"$(top_srcdir)/src/\*.h"

.PHONY: style
style:
if HAVE_ASTYLE_BIN
	$(top_srcdir)/scripts/style.sh --recursive $(ASTYLE_WILDCARDS) \
		--exclude=images/jpeg -i
else
	@echo "configured without astyle"
endif

iconsdir = $(pkgdatadir)
include $(top_srcdir)/icons/Makefile.icons
nobase_dist_icons_DATA = $(PNG_ICONS_FILES)

CPPCHECK_CMD = $(CPPCHECK_BIN) \
	--inline-suppr \
	--suppress=*:*asio/* \
	--config-exclude=include/egt/asio/ \
	--config-exclude=include/egt/asio.hpp \
	--config-exclude=include/egt/detail/floatingpoint.h \
	--error-exitcode=0 \
	-j 4 \
	--force \
	--language=c++ \
	--std=c++11 \
	--platform=unix32 \
	--template=gcc \
	-UTSLIB_MT_VALID \
	-I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/src/images/jpeg

CPPCHECK_DIRS = \
$(top_srcdir)/include/egt/ \
$(top_srcdir)/src/ \
$(top_srcdir)/examples/

cppcheck:
if HAVE_CPPCHECK_BIN
	$(CPPCHECK_CMD) --enable=all $(CPPCHECK_DIRS)
else
	@echo "configured without cppcheck"
endif

lc:
	find $(top_srcdir)/include/ $(top_srcdir)/src/ $(top_srcdir)/examples/ \
		-name '*.h' -o -name '*.c' -o -name '*.cpp' | xargs wc -l

if HAVE_CLOC_BIN
cloc:
	$(CLOC_BIN) --by-file `find $(top_srcdir)/include/ $(top_srcdir)/src/ \
		$(top_srcdir)/examples/ -name '*.h' -o -name '*.c' -o -name '*.cpp'`
	$(CLOC_BIN) `find $(top_srcdir)/include/ $(top_srcdir)/src/ \
		$(top_srcdir)/examples/ -name '*.h' -o -name '*.c' -o -name '*.cpp'`
endif

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
config.h.in \
configure \
depcomp \
install-sh \
missing \
config.guess \
config.rpath \
compile \
config.h.in~ \
config.sub \
ltmain.sh \
test-driver

tidy:
	@cd src && $(MAKE) $(AM_MAKEFLAGS) $@
	@cd examples && $(MAKE) $(AM_MAKEFLAGS) $@

if HAVE_ASPELL_BIN
SPELLCHECK_FILES = `find $(top_srcdir)/docs/src/ $(top_srcdir)/include/ \
	$(top_srcdir)/src/ $(top_srcdir)/examples/ -name '*.h' \
	-o -name '*.c' -o -name '*.cpp' -o -name '*.dox'`

spellcheck:
	for f in $(SPELLCHECK_FILES); do \
		$(ASPELL_BIN) --mode=ccpp --dont-backup --personal=$(top_srcdir)/.aspell.en.pws check $$f; \
	done
endif

checkheaders:
	@cd src && $(MAKE) $(AM_MAKEFLAGS) $@
