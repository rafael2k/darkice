KDOC_DIR=kdoc

SUBDIRS = src man

sysconf_DATA = darkice.cfg

EXTRA_DIST = $(KDOC_DIR) darkice.cfg INSTALL.lame INSTALL.vorbis

DOXYGEN_DIR    = doc/doxygen
DOXYGEN_CONFIG = etc/doxygen.config

.PHONY: doc

doc: src 
	mkdir -p $(DOXYGEN_DIR)
	doxygen etc/doxygen.config

doxygen_clean:
	rm -rf $(DOXYGEN_DIR)

