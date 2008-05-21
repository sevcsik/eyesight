AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS =\
 -I$(top_srcdir) \
 -I$(top_srcdir)/src \ 
 -I$(top_srcdir)/src/bin \
 -DPACKAGE_DATA_DIR=\"$(datadir)/$(PACKAGE)\" \
 -DPACKAGE_PLUGIN_DIR=\"$(libdir)/$(PACKAGE)/plugins\" \
 -DLOCALEDIR=\"$(localedir)\" \
 @EYESIGHT_CFLAGS@

bin_PROGRAMS = eyesight

eyesight_SOURCES = main.c conf.c main_window.c error.c plugin.c
eyesight_LDADD = @EYESIGHT_LIBS@ -ldl


