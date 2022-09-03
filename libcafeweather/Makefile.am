lib_LTLIBRARIES = libcafeweather.la

noinst_PROGRAMS = test_metar test_locations test_sun_moon

AM_CPPFLAGS = 			\
	$(CTK_CFLAGS)		\
	-I$(top_srcdir)		\
	-I$(srcdir)

AM_CFLAGS = $(WARN_CFLAGS)

cafeweather_old_headers = \
	weather.h cafeweather-prefs.h cafeweather-xml.h
cafeweather_new_headers = \
	cafeweather-location.h location-entry.h \
	cafeweather-timezone.h timezone-menu.h
libcafeweatherincdir = $(includedir)/libcafeweather
libcafeweatherinc_HEADERS = \
	$(cafeweather_old_headers)		\
	$(cafeweather_new_headers)		\
	cafeweather-enum-types.h

noinst_HEADERS = weather-priv.h

libcafeweather_la_SOURCES = \
	weather.c weather.h weather-priv.h \
	weather-metar.c weather-iwin.c weather-met.c \
	weather-bom.c weather-wx.c \
	weather-sun.c weather-moon.c \
	cafeweather-enum-types.c \
	cafeweather-prefs.c cafeweather-prefs.h \
	cafeweather-xml.c cafeweather-xml.h \
	cafeweather-location.c cafeweather-location.h \
	cafeweather-timezone.c cafeweather-timezone.h \
	location-entry.c location-entry.h \
	timezone-menu.c timezone-menu.h \
	parser.c parser.h

libcafeweather_la_CPPFLAGS = \
	$(AM_CPPFLAGS)			\
	$(LIBXML_CFLAGS)		\
	$(LIBSOUP_CFLAGS)		\
	$(GIO_CFLAGS)			\
	-DG_LOG_DOMAIN=\"CafeWeather\"	\
	-DCAFELOCALEDIR=\""$(datadir)/locale"\" \
	-DCAFEWEATHER_XML_LOCATION_DIR=\""$(pkgdatadir)"\"

libcafeweather_la_LIBADD = \
	-lm		\
	$(CTK_LIBS)	\
	$(LIBXML_LIBS)	\
	$(LIBSOUP_LIBS)	\
	$(GIO_LIBS) \
	$(REGEX_LIBS)

libcafeweather_la_LDFLAGS = \
	-version-info $(LT_VERSION) -no-undefined

test_metar_SOURCES = test_metar.c
test_metar_CPPFLAGS = $(AM_CPPFLAGS) $(LIBSOUP_CFLAGS)
test_metar_LDADD = libcafeweather.la $(CTK_LIBS)

test_locations_SOURCES = test_locations.c
test_locations_LDADD = libcafeweather.la $(CTK_LIBS)

test_sun_moon_SOURCES = test_sun_moon.c
test_sun_moon_CPPFLAGS = $(AM_CPPFLAGS) $(LIBSOUP_CFLAGS)
test_sun_moon_LDADD = libcafeweather.la $(CTK_LIBS) $(LIBM)

cafeweather-enum-types.h: $(cafeweather_new_headers)
	$(AM_V_GEN)( cd $(srcdir) && $(GLIB_MKENUMS) --template cafeweather-enum-types.h.tmpl \
		$(cafeweather_new_headers) ) > cafeweather-enum-types.h.tmp \
	&& mv cafeweather-enum-types.h.tmp cafeweather-enum-types.h \
	|| rm -f cafeweather-enum-type.h.tmp

cafeweather-enum-types.c: $(libcafeweatherinclude_HEADERS)
	$(AM_V_GEN)( cd $(srcdir) && $(GLIB_MKENUMS) --template cafeweather-enum-types.c.tmpl \
		$(cafeweather_new_headers) ) > cafeweather-enum-types.c.tmp \
	&& mv cafeweather-enum-types.c.tmp cafeweather-enum-types.c \
	|| rm -f cafeweather-enum-type.c.tmp


BUILT_SOURCES = cafeweather-enum-types.c cafeweather-enum-types.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cafeweather.pc

gsettingsschema_in_files = org.cafe.weather.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
@GSETTINGS_RULES@

EXTRA_DIST = cafeweather.pc.in cafeweather-uninstalled.pc.in $(gsettingsschema_in_files)

CLEANFILES = $(noinst_PROGRAMS) $(BUILT_SOURCES) $(gsettings_SCHEMAS) *.gschema.valid

-include $(top_srcdir)/git.mk
