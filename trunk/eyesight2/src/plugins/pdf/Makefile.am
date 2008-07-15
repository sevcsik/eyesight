AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src -I$(top_srcdir)/src/bin \
	 -I$(top_srcdir)/src/plugins/document @EYESIGHT_CFLAGS@ @EPDF_CFLAGS@ \
    @EDVI_CFLAGS@

plugindir = $(prefix)/lib/eyesight/plugins

plugin_LTLIBRARIES = pdf.la dvi.la

pdf_la_SOURCES = document.c animations.c toolbar.c
pdf_la_CFLAGS = -Wall -DPDF
pdf_la_LDFLAGS = -module -avoid-version -no-undefined
pdf_la_LIBTOOLFLAGS = --tag=disable-static
pdf_la_LIBADD = @EYESIGHT_LIBS@ @EPDF_LIBS@ @intl_libs@ @dlopen_libs@

dvi_la_SOURCES = document.c animations.c toolbar.c
dvi_la_CFLAGS = -Wall -DDVI
dvi_la_LDFLAGS = -module -avoid-version -no-undefined
dvi_la_LIBTOOLFLAGS = --tag=disable-static
dvi_la_LIBADD = @EYESIGHT_LIBS@ @EDVI_LIBS@ @intl_libs@ @dlopen_libs@

EXTRA_DIST = animations.h pdf.h document.h toolbar.h
