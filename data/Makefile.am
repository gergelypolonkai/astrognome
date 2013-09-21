SUBDIRS = icons geonames
chartdatadir = $(pkgdatadir)
chartdata_DATA = chart.xsl            \
				 chart.css            \
				 sign_aries.xml       \
				 sign_taurus.xml      \
				 sign_gemini.xml      \
				 sign_cancer.xml      \
				 sign_leo.xml         \
				 sign_virgo.xml       \
				 sign_libra.xml       \
				 sign_scorpio.xml     \
				 sign_sagittarius.xml \
				 sign_capricorn.xml   \
				 sign_aquarius.xml    \
				 sign_pisces.xml      \
				 $(NULL)

gsettings_SCHEMAS = \
					eu.polonkai.gergely.astrognome.gschema.xml \
					$(NULL)
@GSETTINGS_RULES@

EXTRA_DIST = \
			 $(chartdata_DATA)
