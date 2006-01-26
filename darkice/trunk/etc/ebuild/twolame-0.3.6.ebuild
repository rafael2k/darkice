DESCRIPTION="TwoLAME is an optimised MPEG Audio Layer 2 (MP2) encoder"
HOMEPAGE="http://www.twolame.org/"
SRC_URI="mirror://sourceforge/${PN}/${P}.tar.gz"
RESTRICT="nomirror"

SLOT="0"
LICENSE="GPL-2"
KEYWORDS="~x86 ~ppc ~sparc ~alpha ~hppa ~amd64"

DEPEND=">=media-libs/libsndfile-1.0.11"

src_compile() {
	econf || die
	emake || die "Compilation failed"
}

src_install() {
	einstall docdir=${D}/usr/share/doc/${PF} || die

	dodoc AUTHORS ChangeLog COPYING README TODO
}
