geodatadir = $(pkgdatadir)
geodata_DATA = geodata.xml

EXTRA_DIST = geodata.xml

MAINTAINERCLEANFILES = geodata.xml

countryInfoURL  = "http://download.geonames.org/export/dump/countryInfo.txt"
timeZonesURL    = "http://download.geonames.org/export/dump/timeZones.txt"
allCountriesURL = "http://download.geonames.org/export/dump/allCountries.zip"

countryInfo.txt:
	@-if test -x $(WGET); then \
	    $(WGET) $(countryInfoURL); \
	else \
	    if test -x $(CURL); then \
	        $(CURL) $(countryInfoURL) > "$@" ; \
	    else \
	        echo "wget and curl could not be found in your PATH."; \
	        echo "One of them is needed to create geodata.xml!"; \
	    fi; \
	fi

timeZones.txt:
	@-if test -x $(WGET); then \
	    $(WGET) $(timeZonesURL); \
	else \
	    if test -x $(CURL); then \
	        $(CURL) $(timeZonesURL) > "$@" ; \
	    else \
	        echo "wget and curl could not be found in your PATH."; \
	        echo "One of them is needed to create geodata.xml!"; \
	    fi; \
	fi

allCountries.zip:
	@-if test -x $(WGET); then \
	    $(WGET) $(allCountriesURL); \
	else \
	    if test -x $(CURL); then \
	        $(CURL) $(allCountriesURL) > "$@" ; \
	    else \
	        echo "wget and curl could not be found in your PATH."; \
	        echo "One of them is needed to create geodata.xml!"; \
	    fi; \
	fi;

cities.txt: allCountries.zip
	$(AM_V_GEN) if test -x $(UNZIP); then \
	    $(UNZIP) allCountries.zip; \
	else \
	    echo "unzip could not be found in your PATH."; \
	    echo "It is needed to create geodata.xml!"; \
	fi; \
	if test -f allCountries.txt; then \
	    $(AWK) -f geonames_process.awk allCountries.txt > $@ ; \
	fi

geodata.xml: countryInfo.txt timeZones.txt cities.txt
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
