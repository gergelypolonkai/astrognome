resource_files = $(shell glib-compile-resources --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/ag.gresource.xml)

ag-resources.c: ag.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(srcdir) --generate-source --c-name ag $(srcdir)/ag.gresource.xml

ag-resources.h: ag.gresource.xml $(resource_files)
	glib-compile-resources --target=$@ --sourcedir=$(srcdir) --generate-header --c-name ag $(srcdir)/ag.gresource.xml

BUILT_SOURCES = \
				ag-resources.h \
				ag-resources.c \
				$(NULL)

astrognome_source_files = \
						  ag-app.c     \
						  ag-window.c  \
						  ag-chart.c   \
						  astrognome.c \
						  $(NULL)

EXTRA_DIST = \
			 $(resource_files) \
			 ag.gresource.xml \
			 $(NULL)

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"Astrognome\" -DLOCALEDIR=\"$(localedir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
bin_PROGRAMS = astrognome

astrognome_SOURCES = $(astrognome_source_files) $(BUILT_SOURCES)
astrognome_LDADD = $(SWE_GLIB_LIBS) $(GTK_LIBS) $(top_builddir)/libgd/libgd.la
astrognome_LDFLAGS = -rdynamic
astrognome_CFLAGS = $(SWE_GLIB_CFLAGS) $(CFLAGS) $(GTK_CFLAGS) -Wall -I$(top_srcdir)/libgd

guidir = $(pkgdatadir)
gui_DATA = astrognome.ui

EXTRA_DIST += $(gui_DATA)
