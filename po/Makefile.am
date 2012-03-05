DOMAIN = kanatest
POTFILES = $(wildcard ../src/*.c ../src/*.h)

MSGFMT = /usr/bin/msgfmt
XGETTEXT = /usr/bin/xgettext
MSGMERGE = /usr/bin/msgmerge

XGETTEXT_OPTS = --keyword=_ --keyword=N_ --keyword=X_ --add-comments=TRANSLATORS: --no-location
POFILES = de.po el.po es.po fi.po fr.po hu.po it.po pl.po \
		  pt.po ru.po zh_CN.po
MOFILES = $(POFILES:.po=.mo)

EXTRA_DIST = $(POFILES) $(MOFILES)
LOCALEDIR = $(datadir)/locale

all: $(MOFILES)

update-po: $(DOMAIN).pot $(POFILES) $(MOFILES)

%.mo: skip
	@po=$(@:.mo=.po); if test $$po -nt $@ ; then $(MSGFMT) -c --statistics $$po -o $@; echo "$@ updated."; fi

%.po: $(DOMAIN).pot
	$(MSGMERGE) $@ $< -o $@.in && mv $@.in $@

$(DOMAIN).pot: $(POTFILES)
	@$(XGETTEXT) $(XGETTEXT_OPTS) $(POTFILES) -o $(DOMAIN).pot

install-data-local: $(MOFILES)
	for i in $(MOFILES); do \
		lang=`echo $$i | sed 's/\.mo$$//'`; \
		$(mkinstalldirs) ${DESTDIR}${LOCALEDIR}/$$lang/LC_MESSAGES; \
		$(INSTALL_DATA) $$i ${DESTDIR}${LOCALEDIR}/$$lang/LC_MESSAGES/$(DOMAIN).mo; \
	done

uninstall-local:
	for i in $(MOFILES); do \
		lang=`echo $$i | sed 's/\.mo$$//'`; \
		rm -f ${DESTDIR}${LOCALEDIR}/$$lang/LC_MESSAGES/$(DOMAIN).mo; \
	done

clean-local:
	rm -f *.mo

skip:
