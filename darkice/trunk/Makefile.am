KDOC_DIR=kdoc

SUBDIRS = src man

EXTRA_DIST = stl $(KDOC_DIR) darkice.cfg

dist-hook: $(KDOC_DIR)

kdocs: src
	cd src; kdoc --private --strip-h-path --name DarkIce --outputdir ../$(KDOC_DIR)/ *.h

kdocs_clean:
	rm -rf $(KDOC_DIR)

