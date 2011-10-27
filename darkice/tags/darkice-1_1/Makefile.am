DOC_DIR        = doc
DOXYGEN_DIR    = doc/doxygen
DOXYGEN_CONFIG = etc/doxygen.config

SUBDIRS = src man

sysconf_DATA = darkice.cfg

EXTRA_DIST = $(DOC_DIR) darkice.cfg INSTALL.lame INSTALL.vorbis FAQ rc.darkice

.PHONY: doc

doc: src 
#	mkdir -p $(DOXYGEN_DIR)
#	doxygen etc/doxygen.config

doxygen_clean:
	rm -rf $(DOXYGEN_DIR)

