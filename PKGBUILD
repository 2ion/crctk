# Maintainer: 2ion <dev@2ion.de>
pkgname=crctk-git
pkgver=0.5.1_8_g1a46d56
pkgrel=1
pkgdesc="Toolkit for working with CRC32 checksums from the command line"
arch=('i686' 'x86_64')
url="https://github.com/2ion/crctk"
license=('GPL3')
depends=('kyotocabinet' 'zlib' 'ncurses')
makedepends=('git' 'automake' 'autoconf')
provides=("${pkgname%-git}")
conflicts=("${pkgname%-git}")
source=('crctk::git+https://github.com/2ion/crctk.git#branch=master')
md5sums=('SKIP')

pkgver() {
	cd "$srcdir/${pkgname%-git}"
  printf "%s" "$(git describe --tags | tr -- '-' '_')"
}

build() {
	cd "$srcdir/${pkgname%-git}"
  autoreconf --install --force --symlink
	./configure --prefix=/usr
	make
}

package() {
	cd "$srcdir/${pkgname%-git}"
	make DESTDIR="$pkgdir/" install
}
