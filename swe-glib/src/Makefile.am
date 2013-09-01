AM_CPPFLAGS = -DG_LOG_DOMAIN=\"SWE-GLib\" -DLOCALEDIR=\"$(localedir)\" -D__SWE_GLIB_BUILDING__

lib_LTLIBRARIES = libswe-glib-0.1.la

libswe_glib_sources = swe-glib.c gswe-moment.c gswe-timestamp.c enumtypes.c
gswe_headers = gswe-timestamp.h gswe-types.h

libswe_glib_0_1_la_SOURCES = $(libswe_glib_sources)
libswe_glib_0_1_la_CFLAGS = $(GLIB_CFLAGS) $(GOBJECT_CFLAGS) -Wall
libswe_glib_0_1_la_LIBADD = $(GLIB_LIBS) $(GOBJECT_LIBS) $(LIBSWE_LIBS)

BUILT_SOURCES = enumtypes.c enumtypes.h

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = enumtypes.h.template enumtypes.c.template

enumtypes.h: $(gswe_headers) enumtypes.h.template
	$(GLIB_MKENUMS) --template $(filter %.template,$^) $(filter-out %.template,$^) > \
	enumtypes.h.tmp && mv enumtypes.h.tmp enumtypes.h

enumtypes.c: $(gswe_headers) enumtypes.h enumtypes.c.template
	$(GLIB_MKENUMS) --template $(filter %.template,$^) $(filter-out %.template,$^) > \
	enumtypes.c.tmp && mv enumtypes.c.tmp enumtypes.c
