RESOURCE_DIR = $(srcdir)/resources
resource_files = $(shell glib-compile-resources --sourcedir=$(RESOURCE_DIR) --generate-dependencies $(srcdir)/ag.gresource.xml)

ag_enum_headers = ag-icon-view.h ag-header-bar.h

ag-resources.c: ag.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(RESOURCE_DIR) --generate-source --c-name ag $(srcdir)/ag.gresource.xml

ag-resources.h: ag.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(RESOURCE_DIR) --generate-header --c-name ag $(srcdir)/ag.gresource.xml

ag-enumtypes.h: $(ag_enum_headers) ag-enumtypes.h.template
	$(GLIB_MKENUMS) --template $(filter %.template,$^) $(filter-out %.template,$^) > \
	ag-enumtypes.h.tmp && mv ag-enumtypes.h.tmp ag-enumtypes.h

ag-enumtypes.c: $(ag_enum_headers) ag-enumtypes.c.template
	$(GLIB_MKENUMS) --template $(filter %.template,$^) $(filter-out %.template,$^) > \
	ag-enumtypes.c.tmp && mv ag-enumtypes.c.tmp ag-enumtypes.c

BUILT_SOURCES = \
				ag-resources.h \
				ag-resources.c \
				ag-enumtypes.h \
				ag-enumtypes.c \
				$(NULL)

astrognome_source_files = \
						  ag-app.c            \
						  ag-window.c         \
						  ag-chart.c          \
						  ag-settings.c       \
						  ag-preferences.c    \
						  ag-db.c             \
						  ag-display-theme.c  \
						  ag-icon-view.c      \
						  ag-chart-renderer.c \
						  ag-chart-edit.c     \
						  ag-header-bar.c     \
						  astrognome.c        \
						  $(NULL)

EXTRA_DIST = \
			 $(resource_files) \
			 ag.gresource.xml \
			 $(NULL)

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"Astrognome\" -DLOCALEDIR=\"$(localedir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
bin_PROGRAMS = astrognome

astrognome_SOURCES = $(astrognome_source_files) $(BUILT_SOURCES)
astrognome_LDADD = $(SWE_GLIB_LIBS) $(GTK_LIBS) $(LIBXML_LIBS) $(LIBXSLT_LIBS) $(WEBKIT_LIBS) $(GDA_LIBS) $(PIXBUF_LIBS) $(RSVG_LIBS) $(CAIRO_LIBS)
astrognome_LDFLAGS = -rdynamic
astrognome_CFLAGS = $(SWE_GLIB_CFLAGS) $(CFLAGS) $(GTK_CFLAGS) $(LIBXML_CFLAGS) $(LIBXSLT_CFLAGS) $(WEBKIT_CFLAGS) $(GDA_CFLAGS) $(PIXBUF_CFLAGS) $(RSVG_CFLAGS) $(CAIRO_CFLAGS) -Wall

# The following two lines generate a .dir-locals.el file, so
# company-mode won’t die due to unknown includes
.dir-locals.el:
	@echo $(astrognome_CFLAGS) | ./gen-dir-locals-el.sh

.PHONY: .dir-locals.el
