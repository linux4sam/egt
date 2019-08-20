#!/bin/sh

#
# Run through al the snippets and take screenshots.
#

rm -rf screenshots
mkdir screenshots

for label in $(./snippets print labels);
do
    EGT_SCREEN_SIZE=320x240 ./snippets $label &
    sleep 2
    killall -SIGUSR2 lt-snippets
    sleep 1
    mv Window0.png screenshots/${label}.png
    ../../docs/src/dropshadow.sh screenshots/${label}.png
    killall -9 lt-snippets
done
