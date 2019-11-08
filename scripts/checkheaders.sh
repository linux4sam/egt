#!/bin/sh

#
# Test public API headers for missing includes and form
#

includedir=$1
buildincludedir=$2
shift
shift

finish()
{
    rm -f test.cpp test.o
}
trap finish EXIT

for f in $(find $includedir -name '*.h' -printf '%P\n' | sed "s|^$includedir||")
do
    b=$(echo $f | sed 's,\./,,g')
    echo "CHECK $b"

    if grep -q 'config\.h' $includedir/$f; then
	echo "$includedir/$f:0:0: warning: includes config.h"
    fi

    if echo "$f" | grep -q 'egt/detail'; then
       if ! grep -q '#define EGT_DETAIL' $includedir/$f; then
	   echo "$includedir/$f:0:0: warning: bad detail macro"
       fi
    else
       if ! grep -q "<${b}>" $buildincludedir/egt/ui; then
	   echo "$includedir/$f:0:0: warning: not included in <egt/ui>"
       fi
    fi

    echo "#include \"$b\"" > test.cpp
    g++ $@ -c -o test.o test.cpp #|| exit 1
done
