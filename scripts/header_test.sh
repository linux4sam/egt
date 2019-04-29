#!/bin/sh

#
# Test public API headers for missing includes and form
#

top_srcdir=`pwd`/..

CUSTOM_CXXFLAGS="\
-I$top_srcdir \
-I$top_srcdir/include \
-I$top_builddir/include \
-I$top_srcdir/src/images/bmp \
-I$top_srcdir/src/images/jpeg \
-isystem $top_srcdir/external/rapidxml \
-isystem $top_srcdir/external/layout \
-isystem $top_srcdir/external/asio/asio/include \
-DASIO_STANDALONE -DASIO_DISABLE_STD_FUTURE \
-isystem $top_srcdir/external/layout \
-isystem /usr/include/cairo"

rm -f bad.txt

for f in $(cd $top_srcdir/include/ && find . -name '*.h')
do
    b=$(echo $f | sed 's,\./,,g')
    echo $b...

    if grep -q 'config\.h' $top_srcdir/include/$f; then
	echo "$top_srcdir/include/$f:0:0: warning: includes config.h"
    fi

    if echo "$f" | grep -q 'egt/detail'; then
       if ! grep -q '#define EGT_DETAIL' $top_srcdir/include/$f; then
	   echo "$top_srcdir/include/$f:0:0: warning: bad detail macro"
       fi
    else
	if ! grep -q "$b" $top_srcdir/include/egt/ui; then
	   echo "$top_srcdir/include/$f:0:0: warning: not included in <egt/ui>"
       fi
    fi

    echo "#include \"$b\"" > test.cpp
    g++ -std=c++11 $CUSTOM_CXXFLAGS -c -o test.o test.cpp || echo $b >> bad.txt
done
