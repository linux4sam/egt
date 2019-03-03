CUSTOM_CXXFLAGS = $(WARN_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/external/asio/asio/include \
	-DASIO_STANDALONE -DASIO_DISABLE_STD_FUTURE

AM_LDFLAGS = -pthread

bin_PROGRAMS = i18n

MRESG_V_GEN = $(mresg_v_GEN_$(V))
mresg_v_GEN_ = $(mresg_v_GEN_$(AM_DEFAULT_VERBOSITY))
mresg_v_GEN_0 = @echo "  GEN     " $@;

i18n_SOURCES = i18n.cpp
i18n_DEPENDENCIES = build-mo.stamp
i18n_CXXFLAGS = $(LIBPLANES_CFLAGS) $(CAIRO_CFLAGS) $(LIBDRM_CFLAGS) $(TSLIB_CFLAGS) $(LUA_CFLAGS) $(CJSON_CFLAGS) $(GSTREAMER_CFLAGS) $(LIBJPEG_CFLAGS) $(LIBINPUT_CFLAGS) $(LIBRSVG_CFLAGS) $(CUSTOM_CXXFLAGS) $(AM_CXXFLAGS)
i18n_LDADD = $(top_builddir)/src/libegt.la $(LIBPLANES_LIBS) $(CAIRO_LIBS) $(LIBDRM_LIBS) $(TSLIB_LIBS) $(LUA_LIBS) $(CJSON_LIBS) $(GSTREAMER_LIBS) $(LIBJPEG_LIBS) $(LIBINPUT_LIBS) $(LIBRSVG_LIBS)
i18ndir = $(prefix)/share/egt/examples/i18n
i18n_LDFLAGS = $(AM_LDFLAGS)

LINGUAS = de es fr zh

EXTRA_DIST = de.po es.po fr.po zh.po

localedir = $(datadir)/locale

install-data-local: install-i18n-catalogs

install-i18n-catalogs:
	$(MRESG_V_GEN)for i in $(LINGUAS) ; do \
	    $(mkinstalldirs) $(DESTDIR)$(localedir)/$$i.utf8/LC_MESSAGES ; \
	    $(INSTALL_DATA) $$i.utf8/LC_MESSAGES/i18n.mo $(DESTDIR)$(localedir)/$$i.utf8/LC_MESSAGES/i18n.mo ; \
	done

clean-local:
	$(MRESG_V_GEN)for i in $(LINGUAS) ; do \
	    rm -rf $$i.utf8 ; \
	done

uninstall-local:
	rm -rf $(DESTDIR)$(localedir)/*/LC_MESSAGES/poedit.mo

XGETTEXT=xgettext
XGETTEXT_ARGS=-k_ \
	      --package-name=i18nexample --package-version=1.2 \
	      --default-domain=i18nexample
MSGINIT=msginit --no-translator
MSGFMT=msgfmt --no-hash --check
MKDIR=mkdir --parents
MSGMERGE=msgmerge --previous

CLEANFILES = build-mo.stamp i18n.pot

build-mo.stamp:
	$(MRESG_V_GEN)for i in $(LINGUAS); do \
	    $(MKDIR) $$i.utf8/LC_MESSAGES; \
	    $(MSGFMT) --output-file $$i.utf8/LC_MESSAGES/i18n.mo $(top_srcdir)/examples/i18n/$$i.po; \
	done
	@echo Timestamp > $@

build-merge-po: build-pot
	$(MRESG_V_GEN)for i in $(LINGUAS); do \
	    if [ -f $(top_srcdir)/examples/i18n/$$i.po ]; then $(MSGMERGE) $(top_srcdir)/examples/i18n/$$i.po i18n.pot > $$i.new && mv $$i.new $(top_srcdir)/examples/i18n/$$i.po; else cp i18n.pot $(top_srcdir)/examples/i18n/$$i.po; fi \
	done

build-po: build-pot
	$(MRESG_V_GEN)for i in $(LINGUAS); do \
	    $(MSGINIT) --locale $$i --output-file $(top_srcdir)/examples/i18n/$$i.po --input i18n.pot; \
	done
	rm -f build-mo.stamp

build-pot: i18n.cpp
	$(MRESG_V_GEN)$(XGETTEXT) $(XGETTEXT_ARGS) -o i18n.pot $<
