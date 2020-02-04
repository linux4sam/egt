# Rotation Animation Sprite Sheet Generation

generate.sh implements one way of generating a sprite sheet that shows rotation
of an image on its center using the ImageMagick tools.

This takes a single image of a needle and rotates it at degree increments to
generate a bunch of sprites, or frames, for the animation of the needle.  Then,
all of those images are joined together into one big sprite sheet.  This image
can then be loaded right into an egt::Sprite and animated very efficiently using
hardware acceleration.
