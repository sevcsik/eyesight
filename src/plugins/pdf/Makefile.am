AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src -I$(top_srcdir)/src/bin \
	 -I$(top_srcdir)/src/plugins/pdf @EYESIGHT_CFLAGS@ @EPDF_CFLAGS@

plugindir = $(prefix)/lib/eyesight/plugins

plugin_LTLIBRARIES = pdf.la

pdf_la_SOURCES = pdf.c animations.c

pdf_la_CFLAGS = -Wall
pdf_la_LDFLAGS = -module -avoid-version
pdf_la_LIBTOOLFLAGS = --tag=disable-static
pdf_la_LIBADD = @EYESIGHT_LIBS@ @EPDF_LIBS@

EXTRA_DIST = animations.h pdf.h
