#!/bin/bash

# generate a sprite sheet by rotating an image on its center

mkdir .tmp

input=needle.png

deg=0
while [[ $deg -le 359 ]]
do
    name=$(printf %04d $deg)
    convert $input -distort ScaleRotateTranslate ${deg} .tmp/rot_${name}.png
    ((deg = deg + 5))
done

montage .tmp/rot_*.png -tile 8x -geometry 100x100+0+0 -background transparent output.png

rm -rf .tmp
