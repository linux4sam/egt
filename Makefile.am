ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src examples scripts

EXTRA_DIST = README.md COPYING docs/Doxyfile.in

.PHONY: docs

docs: docs/Doxyfile
	@cd docs && doxygen
