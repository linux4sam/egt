#!/bin/sh

convert background.jpg -resize "800x480^" -gravity center -crop 800x480+0+0 +repage background.png
convert background1.jpg -resize "800x480^" -gravity center -crop 800x480+0+0 +repage background1.png
convert background2.jpg -resize "800x480^" -gravity center -crop 800x480+0+0 +repage background2.png

for f in image*.png; do
    convert $f -resize "x180" _${f}
done
