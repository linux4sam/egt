ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src examples scripts

EXTRA_DIST = README.md COPYING docs/Doxyfile.in $(private_icons)

.PHONY: docs style

# consider doing a bit more for Doxygen
# http://chris-miceli.blogspot.com/2011/01/integrating-doxygen-with-autotools.html
docs: docs/Doxyfile
	@cd docs && doxygen

clean-local:
	rm -rf $(top_srcdir)/docs/html
	rm -rf $(top_srcdir)/docs/latex

style:
	./scripts/style.sh --recursive $(top_srcdir)/examples/*.cpp
	./scripts/style.sh $(top_srcdir)/include/mui/*.h $(top_srcdir)/src/*.cpp

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
$(top_srcdir)/include/mui/ \
$(top_srcdir)/src/

cppcheck:
	$(CPPCHECK_CMD) --enable=all $(CPPCHECK_DIRS)
