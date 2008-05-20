AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src -I$(top_srcdir)/src/bin \
	 -I$(top_srcdir)/src/plugins/pdf @EYESIGHT_CFLAGS@

plugindir = $(prefix)/lib/eyesight/plugins

plugin_LTLIBRARIES = pdf.la

pdf_la_SOURCES = pdf.c

pdf_la_LDFLAGS = -module -avoid-version
pdf_la_LIBTOOLFLAGS = --tag=disable-static


