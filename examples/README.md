# EGT Examples

This directory contains various examples showing how to use EGT and how to
exercise EGT functionality.

## Running Examples in Build/Source Directory

Normally, you run `make install` to install the examples and then run them where
they are installed.  When this is done, the examples should be able to find all
of their filesystem resources, the EGT library icons, and the EGT shared library.
Also, you are executing the real binaries when doing this.  However, this is not
always ideal during development and you may wish to run the example right where
it is built which can be in the source tree or in the build tree.

The first thing to note is that libtool wrapper scripts are usually generated
and this is what you execute instead of the real binaries (i.e. examples/gauge)
before they are properly installed. These wrapper scripts, for one, make it
possible to find libegt.so before it's installed.  The real binaries are
usually in `examples/gauge/.libs`.

The second thing is to tell EGT and the example application where to find any
filesystem assets.  The programmer's manual highlights two environment variables
for this purpose: 1) EGT_SEARCH_PATH and 2) EGT_ICONS_DIRECTORY.  So, to run an
example that needs both of these, like gauge, you would do the following to tell
the application where the EGT source files are.

```
cd examples/gauge
EGT_SEARCH_PATH=~/egt/examples/gauge/ EGT_ICONS_DIRECTORY=~/egt/icons ./gauge
```

## License

EGT is released under the terms of the `Apache 2` license. See the [COPYING](../COPYING)
file for more information.
