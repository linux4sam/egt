EXTRA_DIST = \
Doxyfile.in \
custom_footer.html \
custom_header.html \
Doxyfile.in \
extra_stylesheet.css \
footer.tex \
header.tex  \
src \
Info.plist.in

if HAVE_DOXYGEN_BIN
docs: Doxyfile
	$(DOXYGEN_BIN)

DOCSET_INPUT = $(top_srcdir)/docs/html
DOCSET_TAG = $(top_srcdir)/docs/egt.tag
DOCSET_OUTPUT = $(top_srcdir)/docs/EGT.docset
DOCSET_ICON = $(top_srcdir)/docs/src/logo_icon.png
DOCSET_INFO = $(top_srcdir)/docs/Info.plist
DOCSET_TARBALL = EGT.tgz

docset: docs
	mkdir -p $(DOCSET_OUTPUT)/Contents/Resources/Documents/
	cp $(DOCSET_ICON) $(DOCSET_OUTPUT)/icon@2x.png
	convert -resize 16x16 $(DOCSET_ICON) $(DOCSET_OUTPUT)/icon.png
	cp -a $(DOCSET_INPUT)/* $(DOCSET_OUTPUT)/Contents/Resources/Documents/
	cp $(DOCSET_INFO) $(DOCSET_OUTPUT)/Contents/
	doxytag2zealdb --tag $(DOCSET_TAG) \
		--db $(DOCSET_OUTPUT)/Contents/Resources/docSet.dsidx \
		--include-parent-scopes --include-function-signatures
# There's a bug in zeal whhere if the first file it opens is the icon,
# it won't able to see it.  So, when creating the tarball, sort by name
# as a workaround.
	( cd $(top_srcdir)/docs/ && tar --sort=name -cz -f $(DOCSET_TARBALL) EGT.docset/ )
endif

clean-local:
	rm -rf $(top_srcdir)/docs/html
	rm -rf $(top_srcdir)/docs/latex
	rm -f $(DOCSET_TAG)
	rm -rf $(DOCSET_OUTPUT)
	rm -f $(DOCSET_TARBALL)
