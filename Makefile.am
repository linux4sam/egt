ACLOCAL_AMFLAGS = -I m4
SUBDIRS = external src examples scripts

EXTRA_DIST = README.md COPYING docs/Doxyfile.in $(private_icons)

.PHONY: docs style cppcheck

if HAVE_DOXYGEN
docs: docs/Doxyfile
	@cd docs && $(DOXYGEN)
endif

clean-local:
	rm -rf $(top_srcdir)/docs/html
	rm -rf $(top_srcdir)/docs/latex

ASTYLE_WILDCARDS = \
"$(top_srcdir)/examples/\*.cpp" \
"$(top_srcdir)/include/egt/\*.h" \
"$(top_srcdir)/src/\*.c" \
"$(top_srcdir)/src/\*.cpp" \
"$(top_srcdir)/src/\*.h"

style:
	./scripts/style.sh --recursive $(ASTYLE_WILDCARDS) \
		--exclude=images/jpeg -i

private_icons = $(wildcard $(top_srcdir)/icons/*.png)

install-icons:
	for icon in $(private_icons); do \
		mkdir -p $(DESTDIR)$(pkgdatadir)/icons/; \
		$(INSTALL_DATA) $(srcdir)/$$icon $(DESTDIR)$(pkgdatadir)/icons/; \
	done

install-data-local: install-icons


CPPCHECK_CMD = ~/Downloads/cppcheck-1.85/cppcheck \
	--inline-suppr \
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
	$(CPPCHECK_CMD) --enable=all $(CPPCHECK_DIRS)
