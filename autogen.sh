#! /bin/sh

mkdir m4 &> /dev/null

libtoolize --install --copy || exit 1
autoheader || exit 1
aclocal || exit 1
autoconf || exit 1
automake --add-missing --copy || exit 1
