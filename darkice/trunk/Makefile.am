KDOC_DIR=kdoc

SUBDIRS = src man

EXTRA_DIST = stl $(KDOC_DIR) darkice.cfg

$(KDOC_DIR): kdocs

kdocs: src kdocs_clean
	cd src; kdoc --private --strip-h-path --name DarkIce --outputdir ../$(KDOC_DIR)/ *.h

kdocs_clean:
	rm -rf $(KDOC_DIR)

