CUSTOM_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-isystem $(top_srcdir)/external/rapidxml \
	-isystem $(top_srcdir)/external/utfcpp/source \
	$(cairo_CFLAGS) \
	$(CODE_COVERAGE_CXXFLAGS)

CUSTOM_LDADD = $(cairo_LIBS) $(CODE_COVERAGE_LDFLAGS)

bin_PROGRAMS = i18n

MRESG_V_GEN = $(mresg_v_GEN_$(V))
mresg_v_GEN_ = $(mresg_v_GEN_$(AM_DEFAULT_VERBOSITY))
mresg_v_GEN_0 = @echo "  GEN     " $@;

i18n_SOURCES = i18n.cpp
i18n_DEPENDENCIES = build-mo.stamp
i18n_CXXFLAGS = $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
i18n_LDADD = $(top_builddir)/src/libegt.la $(CUSTOM_LDADD)
i18ndir = $(prefix)/share/egt/examples/i18n
i18n_DATA = $(wildcard $(top_srcdir)/examples/i18n/*.png) \
	$(top_srcdir)/examples/i18n/i18n.xml
i18n_LDFLAGS = $(AM_LDFLAGS)

LINGUAS = de es fr zh_CN hi

EXTRA_DIST = de.po es.po fr.po zh_CN.po hi.po i18n.xml

localedir = $(datadir)/locale

install-data-local: install-i18n-catalogs

install-i18n-catalogs:
	$(MRESG_V_GEN)for i in $(LINGUAS) ; do \
	    $(mkinstalldirs) $(DESTDIR)$(localedir)/$$i/LC_MESSAGES || exit $$?; \
	    $(INSTALL_DATA) $$i/LC_MESSAGES/i18n.mo $(DESTDIR)$(localedir)/$$i/LC_MESSAGES/i18n.mo || exit $$?; \
	done

clean-local:
	$(MRESG_V_GEN)for i in $(LINGUAS) ; do \
	    rm -rf $$i ; \
	done

uninstall-local:
	for i in $(LINGUAS) ; do \
	    rm -f $(DESTDIR)$(localedir)/$$i/LC_MESSAGES/i18n.mo ; \
	done

XGETTEXT=xgettext
XGETTEXT_ARGS=-k_ \
	      --package-name=i18n --package-version=1.2 \
	      --default-domain=i18n
MSGINIT=msginit --no-translator
MSGFMT=msgfmt --no-hash --check
MKDIR=mkdir --parents
MSGMERGE=msgmerge --previous

CLEANFILES = build-mo.stamp i18n.pot

build-mo.stamp:
	$(MRESG_V_GEN)for i in $(LINGUAS); do \
	    $(MKDIR) $$i/LC_MESSAGES || exit $$? ; \
	    $(MSGFMT) --output-file=$$i/LC_MESSAGES/i18n.mo $(top_srcdir)/examples/i18n/$$i.po || exit $$? ; \
	done
	@echo Timestamp > $@

build-merge-po: build-pot
	$(MRESG_V_GEN)for i in $(LINGUAS); do \
	    if [ -f $(top_srcdir)/examples/i18n/$$i.po ]; then \
	        $(MSGMERGE) $(top_srcdir)/examples/i18n/$$i.po i18n.pot > $$i.new && mv $$i.new $(top_srcdir)/examples/i18n/$$i.po; \
	    else \
	        cp i18n.pot $(top_srcdir)/examples/i18n/$$i.po; \
	    fi \
	done

build-po: build-pot
	$(MRESG_V_GEN)for i in $(LINGUAS); do \
	    $(MSGINIT) --locale=$$i --output-file=$(top_srcdir)/examples/i18n/$$i.po --input=i18n.pot; \
	done
	rm -f build-mo.stamp

build-pot: i18n.cpp
	$(MRESG_V_GEN)$(XGETTEXT) $(XGETTEXT_ARGS) -o i18n.pot $<

TIDY_FLAGS = $(CUSTOM_CXXFLAGS)
include ../../src/Makefile.tidy
