icons16dir = $(datadir)/icons/hicolor/16x16/apps
icons16_DATA = 16x16/astrognome.png

icons22dir = $(datadir)/icons/hicolor/22x22/apps
icons22_DATA = 22x22/astrognome.png

icons24dir = $(datadir)/icons/hicolor/24x24/apps
icons24_DATA = 24x24/astrognome.png

icons32dir = $(datadir)/icons/hicolor/32x32/apps
icons32_DATA = 32x32/astrognome.png

icons48dir = $(datadir)/icons/hicolor/48x48/apps
icons48_DATA = 48x48/astrognome.png

icons256dir = $(datadir)/icons/hicolor/256x256/apps
icons256_DATA = 256x256/astrognome.png

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
	    echo "Updating Gtk icon cache."; \
	    $(gtk_update_icon_cache); \
	else \
	    echo "*** Icon cache not updated. After (un)install, run this:"; \
	    echo "***   $(gtk_update_icon_cache)"; \
	fi

EXTRA_DIST = \
             $(icon16_DATA)  \
             $(icon22_DATA)  \
             $(icon24_DATA)  \
             $(icon32_DATA)  \
             $(icon48_DATA)  \
             $(icon256_DATA) \
             scalable/astrognome.svg

