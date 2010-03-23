SUBDIRS = src po data
ACLOCAL_AMFLAGS = -I m4 -Wall

if !MAEMO
EXTRA_DIST = README AUTHORS TRANSLATORS COPYING ChangeLog
else
EXTRA_DIST = \
	autogen.sh \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	debian/changelog \
	debian/control \
	debian/copyright \
	debian/rules 
	README \
	AUTHORS \
	TRANSLATORS \
	COPYING \
	ChangeLog
endif
