geodatadir = $(pkgdatadir)
geodata_DATA = geodata.xml

EXTRA_DIST = geodata.xml

MAINTAINERCLEANFILES = geodata.xml

geodata.xml:
	$(AM_V_GEN) if test -x "$(PERL)"; then \
	    if test "x$(have_geonames_perl_modules)" = "xyes" -o "x$(I_HAVE_PERL_MODULES)" = "xyes"; then \
	        $(PERL) geonames_process.pl; \
	    else \
	        echo "XML::Writer and IO::File perl modules are required to process geonames data."; \
	        echo "configure reported they are not installed. If you are sure they are,"; \
	        echo "set the I_HAVE_PERL_MODULES environment variable to yes"; \
	    fi; \
	else \
	    echo "perl5 is required to create geodata.xml!"; \
	fi
