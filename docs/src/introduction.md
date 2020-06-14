@page intro Introduction

[TOC]

@section intro_what_is_egt What is EGT?

The Ensemble Graphics Toolkit (EGT) is a C++ GUI Linux toolkit for
[Microchip](https://www.microchip.com/) AT91/SAMA5 processors.  EGT provides
modern GUI functionality, look-and-feel, and maximizes performance while staying
as close to the metal as possible in embedded Linux applications. EGT provides a
consistent API and solution for working across all Microchip AT91/SAMA5
processors in order to build robust GUI-based applications that make the best
use of the hardware available.

@see For more information about Microchip MPU Xplained or EK type boards, visit
[Linux4SAM](http://www.at91.com/linux4sam/bin/view/Linux4SAM).

EGT uses and provides a standard API to many different third parties, or system
libraries and resources, in order to provide an easy-to-use and consistent
interface for developing a GUI application.  Here are some of them:

@imageSize{egt_libs.png,max-width:90%;height:auto;}
@image html egt_libs.png "Integration"
@image latex egt_libs.png "Integration"


@section intro_what What is a Graphics Toolkit?

A graphics toolkit (also known as a Graphical User Interface (GUI) framework) is
used to construct graphical user interfaces of applications.  It provides a
well defined API and an organization of features, properties, and methods that
makes creating user interfaces easier and more consistent. This can mean
creating an API to the operating system or various system libraries.  In
addition, a toolkit may provide various tools that make creating graphical
applications easier for both programmers and graphic artists.

In order to accomplish this, EGT contains several different layers of software,
with the application code expected to be sitting on top.

@imageSize{high_level.png,width:500px;}
@image html high_level.png "High level"
@image latex high_level.png "High level" width=8cm

@section intro_history Architecture

A software architecture involving EGT usually involves using EGT as the middle
layer between the application code and everything else.  EGT provides an
abstraction layer to everything below it to an application programmer.

@imageSize{architecture.png,width:500px;}
@image html architecture.png "Architecture"
@image latex architecture.png "Architecture" width=8cm

In order to provide this abstraction, EGT provides a list of objects, or C++
classes, some of which are specialized into a type called egt::Widget. The @ref
controls are your standard interface components.  @ref sizers widgets help with
layout and position of other widgets.  There are some basic @ref shapes widgets
for drawing plain shapes.  And of course, the @ref media widgets handle playing
audio and video related media.

And then, there are some framework and application level classes like
egt::Application, egt::EventLoop, egt::Input, egt::Screen, @ref timers, and
egt::Event that help everything work together.

@section intro_features EGT Features

This is a list of high level features generally supported and provided by EGT:

- Anti-aliased 2D vector graphics.
- Modern C++ design and simplicity.
- Optimized for Microchip microprocessors running Linux.
- DRM/KMS, X11, and fbdev backends.
- libinput, tslib, and evdev input support for mouse, keyboard, and touchscreen.
- UTF-8 encoding with internationalization and localization support.
- Support for libplanes and hardware LCD overlay planes.
- Built in and simple animation support for creating custom effects.
- A rich and extensible default widget set designed for embedded touchscreens.
- Multimedia support for video playback, raster images, and vector graphics.
- Customizable look and feel with theme-able widget drawing, colors, and fonts.

@section intro_deps EGT Dependencies

EGT depends on a variety of different third party libraries.  Some of them are
required or recommended, and others are optional for conditional features in
EGT.  EGT is configurable so you can remove support for third party libraries if
they are not needed.

Library              | Dependency        | Notes
-------------------- | ----------------- | --------------
libplanes >= 1.0.0   | Required (Target) | Access to Microchip DRM/KMS hardware driver.
libdrm >= 2.4.0      | Required (Target) | Access to DRM/KMS hardware drivers.
cairo >= 1.14.6      | Required          | 2D drawing library.
fontconfig           | Required          | Support for system level font configuration.
x11 >= 1.6.3         | Required (Host)   | Support for X11 display server.
libinput >= 1.6.3    | Recommended       | Support for reading input devices (keyboard, touchscreen, mouse).
libjpeg              | Recommended       | Support for JPEG content.
libmagic             | Recommended       | Support for automatically detecting mime types of content.
zlib                 | Recommended       | Support for zlib compression.
libpng               | Recommended       | Support for PNG content.
lua >= 5.3.1         | Optional          | Support for lua bindings and built in lua interpreter.
xkbcommon            | Optional          | Support for system configured key mappings.
tslib >= 1.15        | Optional          | Support for some touchscreen events.
gstreamer-1.0 >= 1.8 | Optional          | Video, audio, camera playback.
libcurl >= 4.5       | Optional          | Built in application level networking protocols.
librsvg-2.0          | Optional          | SVG rendering support.
libsndfile           | Optional          | Support for parsing wav files.
alsa                 | Optional          | Support for audio playback.
plplot               | Optional          | Charting support.

The `<egt/ui>` file provides several macros to compile time check what is support
in EGT based on conditional support.  For example, `EGT_HAS_SVG` and `EGT_HAS_CHART`
are defined when those features are available.

@section intro_internal_deps EGT Internal Dependencies

EGT also unconditionally includes some third party developed libraries.  These
libraries are included internally by EGT, however the capabilities they provide
can sometimes be controlled by EGT compilation options.

- [cxxopts](https://github.com/jarro2783/cxxopts) - Lightweight C++ command line option parser
- [googletest](https://github.com/google/googletest) - Google Testing and Mocking Framework
- [layout](https://github.com/randrew/layout) - Single-file library for calculating 2D UI layouts using stacking boxes
- [rapidxml](http://rapidxml.sourceforge.net/) - Fast XML parser
- [Simd](https://github.com/ermig1979/Simd) - C++ image processing and machine learning library with using of SIMD
- [spdlog](https://github.com/gabime/spdlog) - Fast C++ logging library
- [utfcpp](http://utfcpp.sourceforge.net/) - UTF-8 with C++ in a Portable Way

@section intro_language C++ Language and Namespaces

EGT takes advantage of and focuses on using features available in the C++14
language standard.  The EGT API is all inside a ::egt namespace.  This
namespace contains an inline namespace ::egt::v1 which is used for versioning
the API in the future.

Within the ::egt namespace, there are several other namespaces that have
different purposes and meaning.

Namespace                 | Meaning
------------------------- | -------------
::egt                     | Default namespace.
::egt::v1                 | Inline namespace denoting the API version.
::egt::v1::detail         | A namespace usually used to hide internal EGT functionality.  This is not considered stable or public.
::egt::v1::experimental   | A namespace for what is considered unstable and experimental.

@remark Did you know the C++ Standard Draft Sources are [available on GitHub](https://github.com/cplusplus/draft)?
