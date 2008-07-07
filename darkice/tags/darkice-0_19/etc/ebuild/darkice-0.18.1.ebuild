# Copyright 1999-2006 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header$

DESCRIPTION="IceCast live streamer, delivering ogg and mp3 streams simultaneously to multiple hosts."
HOMEPAGE="http://darkice.tyrell.hu/"
SRC_URI="http://darkice.tyrell.hu/dist/${PV}/${P}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~alpha ~amd64 ~hppa ~ppc ~sparc ~x86"
IUSE="alsa encode jack vorbis"

DEPEND="encode?	( >=media-sound/lame-1.89 )
	vorbis? ( >=media-libs/libvorbis-1.0 )
	alsa? ( >=media-libs/alsa-lib-1.0.0 )
	jack? ( media-sound/jack-audio-connection-kit )"

src_compile() {
	if ! use encode && ! use vorbis
	then

		eerror "You need support for mp3 or Ogg Vorbis enconding for this"
		eerror "package. Please merge again with at least one of the "
		eerror "\`encode' and \`vorbis' USE flags enabled:"
		eerror
		eerror "  # USE=\"encode\" emerge darkice"
		eerror "  # USE=\"vorbis\" emerge darkice"
		die "Won't build without support for lame nor vorbis"
	fi

	econf $(use_with alsa) \
	      $(use_with encode lame) \
		  $(use_with jack) \
	      $(use_with vorbis) || die "configuration failed"
	emake || die "Compilation failed"
}

src_install() {
	einstall darkicedocdir=${D}/usr/share/doc/${PF} || die "make install failed"

	dodoc AUTHORS ChangeLog NEWS README TODO
}
