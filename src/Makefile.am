AM_CPPFLAGS = -DG_LOG_DOMAIN=\"Astrognome\" -DLOCALEDIR=\"$(localedir)\" -DPKGDATADIR=\"$(pkgdatadir)\"
bin_PROGRAMS = astrognome

astrognome_SOURCES = astrognome.c
astrognome_LDADD = $(SWE_GLIB_LIBS) $(GTK_LIBS)
astrognome_LDFLAGS = -rdynamic
astrognome_CFLAGS = $(SWE_GLIB_CFLAGS) $(CFLAGS) $(GTK_CFLAGS) -Wall

guidir = $(pkgdatadir)
gui_DATA = astrognome.ui

EXTRA_DIST = $(gui_DATA)
