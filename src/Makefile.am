if !MAEMO
REVISION := $(shell if test -e .svn; then echo -DREV=\"`LC_ALL=C svn info | sed -n '/^Rev/p'| sed -e 's/^Revision:\ //'`\"; fi;)
AM_CPPFLAGS = -Wall -DLOCALEDIR=\"$(datadir)/locale\" $(REVISION) \
			  -DGDK_PIXBUF_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED
bin_PROGRAMS = kanatest
kanatest_SOURCES = about.c about.h \
				   chart.c chart.h \
				   graphics.h \
				   gui.c gui.h \
				   gui_utils.c gui_utils.h \
				   i18n.h \
				   main.c main.h \
				   options.c options.h \
				   prefs.c prefs.h \
				   stats.c stats.h \
				   stock_icons.c stock_icons.h stock_icons_data.h \
				   test.c test.h 

kanatest_LDADD = $(LIBINTL)

else

bin_PROGRAMS = kanatest

kanatest_LDADD = $(gtk_LIBS) $(xml_LIBS) $(HILDON_LIBS) $(OSSO_LIBS)

kanatest_CFLAGS= $(gtk_CFLAGS) $(xml_CFLAGS) $(HILDON_CFLAGS) $(OSSO_CFLAGS)

kanatest_SOURCES = about.c about.h \
				   chart.c chart.h \
				   graphics.h \
				   gui.c gui.h \
				   gui_utils.c gui_utils.h \
				   i18n.h \
				   main.c main.h \
				   options.c options.h \
				   prefs.c prefs.h \
				   stats.c stats.h \
				   stock_icons.c stock_icons.h stock_icons_data.h \
				   test.c test.h 

endif
