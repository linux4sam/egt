# How to build different resolution demo of motorcycledash

This demo supports 2 kinds of resolution of screen:
- 800x480
- 1024x600

The eraw.h and eraw.bin are for 800x480 resolution, and the eraw.bin.1024x600
and eraw.h.1024x600 are for 1024x600 resolution. So when you build by default
eraw.h, the generated app is 800x480 resolution, at the same time, you need to
copy the eraw.bin to target to read by app.

If you want to build 1024x600 resolution, please rename eraw.h to eraw.h.800x480
and rename eraw.h.1024x600 to eraw.h, and then make this example, at the same
time, you need to copy the eraw.bin.1024x600 to target and rename it to
eraw.bin to read by app.


## License

EGT is released under the terms of the `Apache 2` license. See the [COPYING](../COPYING)
file for more information.
