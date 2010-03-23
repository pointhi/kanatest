if !MAEMO

SUBDIRS = icons pixmaps

applicationsdir = $(datadir)/applications
applications_DATA = kanatest.desktop

EXTRA_DIST = $(applications_DATA)

install-data-hook:
uninstall-hook:

else

serviceentry_DATA = maemo/org.clayo.kanatest.service
desktopentry_DATA = maemo/kanatest.desktop

icon_26x26_DATA = maemo/icons/26x26/kanatest.png 
icon_40x40_DATA = maemo/icons/40x40/kanatest.png
icon_scalable_DATA = maemo/icons/scalable/kanatest.png

EXTRA_DIST = $(serviceentry_DATA) $(desktopentry_DATA) $(icon_26x26_DATA) $(icon_40x40_DATA) $(icon_scalable_DATA)

install-data-hook:
uninstall-hook:

endif
