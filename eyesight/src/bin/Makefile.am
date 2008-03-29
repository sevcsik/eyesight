AUTOMAKE_OPTIONS     = 1.4 foreign

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src -I$(top_srcdir)/src/bin @EYESIGHT_CFLAGS@

bin_PROGRAMS = eyesight

eyesight_SOURCES = main.c init_libs.c conf.c main_window.c error.c
eyesight_LDADD = @EYESIGHT_LIBS@ -ldl


