KDOC_DIR=kdoc

SUBDIRS = src man

sysconf_DATA = darkice.cfg

EXTRA_DIST = $(KDOC_DIR) darkice.cfg INSTALL.lame INSTALL.vorbis

$(KDOC_DIR): kdocs

kdocs: src kdocs_clean
	cd src; kdoc --private --strip-h-path --name DarkIce --outputdir ../$(KDOC_DIR)/ *.h

kdocs_clean:
	rm -rf $(KDOC_DIR)

