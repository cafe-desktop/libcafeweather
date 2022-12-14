## Process this file with automake to produce Makefile.in

# We require automake 1.6 at least.
AUTOMAKE_OPTIONS = 1.6

# The name of the module, e.g. 'glib'.
DOC_MODULE=libcafeweather

# Uncomment for versioned docs and specify the version of the module, e.g. '2'.
#DOC_MODULE_VERSION=2

# The top-level SGML file. You can change this if you want to.
DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.sgml

# The directory containing the source code. Relative to $(srcdir).
# gtk-doc will search all .c & .h files beneath here for inline comments
# documenting the functions and macros.
# e.g. DOC_SOURCE_DIR=../../../ctk
DOC_SOURCE_DIR=../libcafeweather

# Extra options to pass to gtkdoc-scangobj. Not normally needed.
SCANGOBJ_OPTIONS=

# Extra options to supply to gtkdoc-scan
# e.g. SCAN_OPTIONS=--deprecated-guards="CTK_DISABLE_DEPRECATED"
SCAN_OPTIONS=--deprecated-guards="CAFEWEATHER_DISABLE_DEPRECATED"

# Extra options to supply to gtkdoc-mkdb.
# e.g. MKDB_OPTIONS=--sgml-mode --output-format=xml
MKDB_OPTIONS=--sgml-mode --output-format=xml

# Extra options to supply to gtkdoc-mktmpl
# e.g. MKTMPL_OPTIONS=--only-section-tmpl
MKTMPL_OPTIONS=

# Extra options to supply to gtkdoc-mkhtml
MKHTML_OPTIONS=

# Extra options to supply to gtkdoc-fixref. Not normally needed.
# e.g. FIXXREF_OPTIONS=--extra-dir=../gdk-pixbuf/html --extra-dir=../cdk/html
FIXXREF_OPTIONS=

# Used for dependencies. The docs will be rebuilt if any of these change.
# e.g. HFILE_GLOB=$(top_srcdir)/ctk/*.h
# e.g. CFILE_GLOB=$(top_srcdir)/ctk/*.c
HFILE_GLOB=$(top_srcdir)/libcafeweather/*.h
CFILE_GLOB=$(top_srcdir)/libcafeweather/*.c

# Header files to ignore when scanning.
# e.g. IGNORE_HFILES=ctkdebug.h ctkintl.h
IGNORE_HFILES=cafeweather-enum-types.h parser.h weather-priv.h

# Images to copy into HTML directory.
# e.g. HTML_IMAGES=$(top_srcdir)/ctk/stock-icons/stock_about_24.png
HTML_IMAGES=

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE).
# e.g. content_files=running.sgml building.sgml changes-2.0.sgml
content_files=

# SGML files where gtk-doc abbrevations (#CtkWidget) are expanded
# These files must be listed here *and* in content_files
# e.g. expand_content_files=running.sgml
expand_content_files=

# CFLAGS and LDFLAGS for compiling gtkdoc-scangobj with your library.
# Only needed if you are using gtkdoc-scangobj to dynamically query widget
# signals and properties.
# e.g. GTKDOC_CFLAGS=-I$(top_srcdir) -I$(top_builddir) $(CTK_DEBUG_FLAGS)
# e.g. GTKDOC_LIBS=$(top_builddir)/ctk/$(ctktargetlib)
GTKDOC_CFLAGS = -I$(top_srcdir) $(CTK_CFLAGS) $(LIBXML_CFLAGS) $(LIBSOUP_CFLAGS) -DCAFEWEATHER_I_KNOW_THIS_IS_UNSTABLE
GTKDOC_LIBS = $(top_builddir)/libcafeweather/libcafeweather.la $(CTK_LIBS) $(LIBXML_LIBS) $(LIBSOUP_LIBS)

# This includes the standard gtk-doc make rules, copied by gtkdocize.
include $(top_srcdir)/gtk-doc.make

# Other files to distribute
# e.g. EXTRA_DIST += version.xml.in
EXTRA_DIST +=

# Files not to distribute
# for --rebuild-types in $(SCAN_OPTIONS), e.g. $(DOC_MODULE).types
# for --rebuild-sections in $(SCAN_OPTIONS) e.g. $(DOC_MODULE)-sections.txt
#DISTCLEANFILES +=

# Comment this out if you want your docs-status tested during 'make check'
#TESTS = $(GTKDOC_CHECK)


-include $(top_srcdir)/git.mk
