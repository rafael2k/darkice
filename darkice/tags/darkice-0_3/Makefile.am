SUBDIRS = src
EXTRA_DIST = stl doc darkice.cfg

dist-hook: kdocs

kdocs: src
	cd src; kdoc --private --strip-h-path --name DarkIce --outputdir ../doc/ *.h

kdocs_clean:
	rm -rf doc/

