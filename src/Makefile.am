RESOURCE_DIR = $(srcdir)/resources
resource_files = $(shell glib-compile-resources --sourcedir=$(RESOURCE_DIR) --generate-dependencies $(srcdir)/ag.gresource.xml)

ag-resources.c: ag.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(RESOURCE_DIR) --generate-source --c-name ag $(srcdir)/ag.gresource.xml

ag-resources.h: ag.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(RESOURCE_DIR) --generate-header --c-name ag $(srcdir)/ag.gresource.xml

BUILT_SOURCES = \
				ag-resources.h \
				ag-resources.c \
				$(NULL)

astrognome_source_files = \
						  ag-app.c            \
						  ag-window.c         \
						  ag-chart.c          \
						  ag-settings.c       \
						  ag-preferences.c    \
						  ag-db.c             \
						  ag-display-theme.c  \
						  astrognome.c        \
						  $(NULL)

EXTRA_DIST = \
			 $(resource_files) \
			 ag.gresource.xml \
			 $(NULL)

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"Astrognome\" -DLOCALEDIR=\"$(localedir)\" -DPKGDATADIR=\"$(pkgdatadir)\" -I$(top_srcdir)/libgd
bin_PROGRAMS = astrognome

astrognome_SOURCES = $(astrognome_source_files) $(BUILT_SOURCES)
astrognome_LDADD = $(SWE_GLIB_LIBS) $(GTK_LIBS) $(LIBXML_LIBS) $(LIBXSLT_LIBS) $(WEBKIT_LIBS) $(GDA_LIBS) $(PIXBUF_LIBS) $(RSVG_LIBS) $(top_builddir)/libgd/libgd.la
astrognome_LDFLAGS = -rdynamic
astrognome_CFLAGS = $(SWE_GLIB_CFLAGS) $(CFLAGS) $(GTK_CFLAGS) $(LIBXML_CFLAGS) $(LIBXSLT_CFLAGS) $(WEBKIT_CFLAGS) $(GDA_CFLAGS) $(PIXBUF_CFLAGS) $(RSVG_CFLAGS) -Wall

