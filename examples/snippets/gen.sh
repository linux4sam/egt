#!/bin/bash

#
# Generate images of all of the code snippets.
# Requires pygmentize.
#

rm -rf generated
mkdir generated

function do_gen()
{
    tmp=generated/${2}.cpp

    sed -n "/\[${label}\]/,/\[${label}\]/{/\[${label}\]/b;/\[${label}\]/b;p}" $1 > $tmp

    ../../scripts/style.sh $tmp < /dev/null

    pygmentize -f html -O style=default,full=1 -l cpp -o ${tmp}.html $tmp
    pygmentize -f png -O style=default -l cpp -o ${tmp}.png $tmp
    pygmentize -f svg -O style=default -l cpp -o ${tmp}.svg $tmp
    ../../docs/src/dropshadow.sh ${tmp}.png

    lines=$(wc -l $tmp | cut -d' ' -f1)
    for (( i=0; i<$lines; i++ ))
    do
	pygmentize -f html -O style=default,full=1,hl_lines=$i -l cpp -o ${tmp}${i}.html $tmp
	pygmentize -f png -O style=default,hl_lines=$i -l cpp -o ${tmp}${i}.png $tmp
	pygmentize -f svg -O style=default,hl_lines=$i -l cpp -o ${tmp}${i}.svg $tmp
	../../docs/src/dropshadow.sh ${tmp}${i}.png
    done
}

for label in $(./snippets print labels);
do
    do_gen snippets.cpp $label
done

for label in snippet0 snippet1 snippet2 snippet3 snippet4 snippet5 snippet6 snippet7 snippet8 snippet9;
do
    do_gen $label.cpp $label
done
