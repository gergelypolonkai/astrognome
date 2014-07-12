SUBDIRS = icons geonames

gsettings_SCHEMAS = \
					eu.polonkai.gergely.astrognome.gschema.xml \
					$(NULL)
@GSETTINGS_RULES@

@INTLTOOL_DESKTOP_RULE@

desktopdir = $(datadir)/applications
desktop_in_files = astrognome.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

EXTRA_DIST = \
			 $(desktop_in_files)  \
			 $(gsettings_SCHEMAS) \
			 $(NULL)

CLEANFILES = \
			 $(DESKTOP_FILES) \
			 $(NULL)

DISTCLEANFILES = \
				 $(desktop_DATA) \
				 $(NULL)
