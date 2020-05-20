# EGT Examples

Probably the best way to learn EGT is by reading the source of example code.
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

## Examples

- audioplayer

  Demonstrates use of AudioPlayer to play an mp3, wav, ogg, or more audio files
  with seek, pause, play, and progress indicators.

- basic

  The most basic example.

- basicui

  Loads a UI XML file and renders it.

- boards

  Shows how to use the SideBoard helper widget to animate windows sliding on and
  off the screen

- brick

  Simple brick style game with collision detection and animations.

- calc

  Simple example that implements a basic calculator by using Lua as the
  expression evaluator.

- charts

  Demonstration of various chart types and properties of charts.

- colors

  Shows what colors look like using various ColorMap instances with different
  color spaces.

- dash

  Uses the Gauge class to implement a basic car dashboard with animations and
  events.

- dialog

  Demonstration of various dialog helper classes available.

- drag

  Shows how to implement dragging of objects on the screen.

- drummachine

  Basic drum machine that uses the Sound class to asynchronously and quickly
  play wav sound files based on button events.

- easing

  Shows an animation and a chart for built-in animation easing functions for
  easy visualization of what an easing curve "looks" like.

- floating

  Shows animated floating objects that overlap.  The objects can also be
  manually dragged around the screen.

- frames

  Test showing the position of nested frames.

- gpio

  Shows how to integrate reading raw gpio events asynchronously by using the EGT
  event loop.

- guage

  Demonstrates various uses of the Gauge and GaugeLayer classes.

- i18n

  Demonstrates using various languages and fonts, including emoji fonts.

- imagestack

  Shows a navigation example of images with animation effects.

- keys

  Shows key events seen by EGT for any input device.

- luarepl

- monitor

  Demonstrates charting a continuous stream of data with various chart types.

- press

  Shows touchscreen events.

- resource

  Loads various image resources of different types from different locations.

- sizers

  Test for various nested sizers on the screen.

- snippets

  Generally not complete examples, but small and various code snippets
  demonstrating how to use some classes and set properties.

- space

  Demonstrates responding to touchscreen events and creating animations of
  objects across the screen.

- sprite

  Shows how to use the Sprite class to animate sprite sheets with and without
  hardware acceleration.

- squares

  Benchmark that shows drawing various sizes of squares with and without alpha blending.

- video

  Collection of examples for playing video and showing or capturing camera feeds.

- virtualkeyboard

  Shows how to use the VirtualKeyboard class to create custom on-screen
  keyboards and keypads.

- whiteboard

  Shows custom drawing and filling based on touchscreen events.  Also shows how
  to create a custom Popup window for configuration options.

- widgets

  Shows what all controls looks like in various states with the ability to apply
  different themes globally.

## License

EGT is released under the terms of the `Apache 2` license. See the [COPYING](../COPYING)
file for more information.
