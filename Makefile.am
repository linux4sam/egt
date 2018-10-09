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
	./scripts/style.sh --recursive examples/*.cpp
	./scripts/style.sh include/mui/*.h src/*.cpp

private_icons = $(wildcard $(top_srcdir)/icons/*.png)

install-icons:
	for icon in $(private_icons); do \
		mkdir -p $(DESTDIR)$(pkgdatadir)/icons/; \
		$(INSTALL_DATA) $(srcdir)/$$icon $(DESTDIR)$(pkgdatadir)/icons/; \
	done

install-data-local: install-icons
