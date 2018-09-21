ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src examples scripts

EXTRA_DIST = README.md COPYING docs/Doxyfile.in $(private_icons)

.PHONY: docs style

docs: docs/Doxyfile
	@cd docs && doxygen

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
