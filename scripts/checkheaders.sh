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

# c++11 and c++14 headers
allowed_headers="<cstdlib>
<csignal>
<csetjmp>
<cstdarg>
<typeinfo>
<typeindex>
<type_traits>
<bitset>
<functional>
<utility>
<ctime>
<chrono>
<cstddef>
<initializer_list>
<tuple>
<new>
<memory>
<scoped_allocator>
<climits>
<cfloat>
<cstdint>
<cinttypes>
<limits>
<exception>
<stdexcept>
<cassert>
<system_error>
<cerrno>
<cctype>
<cwctype>
<cstring>
<cwchar>
<cuchar>
<string>
<array>
<vector>
<deque>
<list>
<forward_list>
<set>
<map>
<unordered_set>
<unordered_map>
<stack>
<queue>
<iterator>
<algorithm>
<cmath>
<complex>
<valarray>
<random>
<numeric>
<ratio>
<cfenv>
<iosfwd>
<ios>
<istream>
<ostream>
<iostream>
<fstream>
<sstream>
<iomanip>
<streambuf>
<cstdio>
<locale>
<clocale>
<codecvt>
<regex>
<atomic>
<thread>
<mutex>
<shared_mutex>
<future>
<condition_variable>"

# basically anything deprecated by c++11 or > c++14 is not allowed
disallowed_headers="
<any>
<optional>
<variant>
<compare>
<version>
<source_location>
<memory_resource>
<string_view>
<charconv>
<format>
<span>
<ranges>
<execution>
<bit>
<syncstream>
<strstream>
<stop_token>
<semaphore>
<latch>
<barrier>
<filesystem>"

for f in $(find $includedir -name '*.h' -printf '%P\n' | sed "s|^$includedir||")
do
    b=$(echo $f | sed 's,\./,,g')
    echo "CHECK $b"

    # no config.h allowed
    if grep -q 'config\.h' $includedir/$f; then
	echo "$includedir/$f:0:0: error: includes config.h"
    fi

    # deprecated or too new or otherwise disallowed headers
    for header in $disallowed_headers; do
	if grep -q "\#include $header" $includedir/$f; then
	    echo "$includedir/$f:0:0: error: disallowed header $header"
	fi
    done

    # detect any non-standard language or egt headers
    grep '^#include ' $includedir/$f | while read -r line ; do
	head=$(echo "$line" | sed 's/\#include //g')
	head_bare=$(echo "$head" | sed 's/[<>]//g')
	if ! echo "$allowed_headers" | grep -q "$head"; then
	    if ! test -f $includedir/$head_bare; then
		echo "$includedir/$f:0:0: warning: reduce external dependency header $head"
	    fi
	fi
    done

    if echo "$f" | grep -q 'egt/detail'; then
        # check include guard name for detail headers
	if ! grep -q '#define EGT_DETAIL' $includedir/$f; then
	   echo "$includedir/$f:0:0: warning: bad detail macro"
       fi
    else
       # file not included in main egt header
       if ! grep -q "<${b}>" $buildincludedir/egt/ui; then
	   echo "$includedir/$f:0:0: notice: not included in <egt/ui>"
       fi
    fi

    # compile test by *just* including the header in question
    # to find any missing dependent headers
    echo "#include \"$b\"" > test.cpp
    g++ $@ -c -o test.o test.cpp #|| exit 1
done
