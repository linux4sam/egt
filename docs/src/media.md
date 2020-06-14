@page media_page Media

[TOC]

This chapter discusses media formats supported by EGT.

@section media_intro Supported Media Formats

EGT has direct and indirect support for many standard image, video, and
audio formats through classes like egt::VideoWindow, egt::AudioPlayer,
egt::CameraWindow, egt::v1::experimental::CameraCapture, egt::v1::experimental::Sound,
egt::Image, egt::SvgImage, and more.  In most cases, the low level handling is
implemented using third party libraries like libpng, libjpeg, GStreamer, librsvg,
Video4Linux2, and so on.

@section media_image Image Formats

The following image formats are directly supported by EGT:
- PNG
  - Raster graphics file format that supports lossless data compression.
- JPEG
  - Lossy compression for digital images.
- BMP
  - Raster graphics file format used to store bitmap digital images.
- SVG
  - Scalable Vector Graphics is an Extensible Markup Language (XML)-based vector
  image format for two-dimensional graphics.

Loading and using any image in EGT is automatically done through either the
egt::Image or egt::SvgImage classes.

@snippet "../examples/snippets/snippets.cpp" image1

@section media_svg Scalable Vector Graphics Files

Scalable Vector Graphics (SVG) is an XML-based vector image format for
two-dimensional graphics. The SVG specification is an open standard developed
by the World Wide Web Consortium (W3C) since 1999. SVG images and their
behaviors are defined in XML text files.

EGT can load SVG files using the third party library
[librsvg](https://github.com/GNOME/librsvg).  librsvg is a library to render SVG
files using cairo.  On top of that, this library provides the ability to load
elements by id from SVG files.  What this means is a graphic designer can create
an SVG file and give each component of the UI a unique element ID.  Then, an EGT
programmer can individually load these components and assign them to widgets and
create logic around these components from a single SVG file.  Also, a graphic
designer can put elements in the SVG that are a hint where EGT should be used to
draw something, like text.

@note EGT does not support all SVG capabilities including things like
extensions, javascript, or animations.

@note While EGT supports loading SVG files and working with them dynamically, it
is also worth noting that there [are tools](https://github.com/akrinke/svg2cairo)
that can convert an SVG file into actual cairo code.  This may be useful for
several reasons, including reducing dependencies of EGT.

@note Example SVG files can be found
[on various websites](https://dev.w3.org/SVG/tools/svgweb/samples/svg-files/).

Working with an SVG file in EGT is mostly accomplished with the egt::SvgImage class.
This class allows fine grained access into SVG specific properties,
while still allowing easy conversion to a normal raster Image instance.

egt::v1::experimental::Gauge, egt::v1::experimental::GaugeLayer, and
egt::v1::experimental::NeedleLayer are several classes that are useful for taking
advantage of SVG files by using complete SVG files or individual objects in
those SVG files to construct layered widgets.

@section media_gstreamer GStreamer

EGT uses [GStreamer](https://gstreamer.freedesktop.org) as the default backend
to implement audio, video, and camera playback or capture.  In addition,
Microchip provides [custom GStreamer plugins](https://github.com/linux4sam/gst1-hantro-g1)
for hardware acceleration of video and image decoding available on some
processors.

GStreamer is a library for constructing graphs of media-handling components. The
applications it supports range from simple Ogg/Vorbis playback, audio/video
streaming to complex audio (mixing) and video (non-linear editing) processing.
Applications can take advantage of advances in codec and filter technology
transparently. Developers can add new codecs and filters by writing a simple
plugin with a clean, generic interface.

Because EGT uses GStreamer for audio, video, and camera support, the features
and capabilities of GStreamer are in turn extended to EGT:

@par Container Formats
asf, avi, 3gp/mp4/mov, flv, mpeg-ps/ts, mkv/webm, mxf, ogg
@par Streaming
http, mms, rtsp
@par Codecs
FFmpeg, various codec libraries, 3rd party codec packs
@par Metadata
Native container formats with a common mapping between them
@par Video
 Various colorspaces, support for progressive and interlaced video
@par Audio
Integer and float audio in various bit depths and multichannel configurations including:
 - Raw audio formats
 - Encoded audio formats: AC-3 or A52 audio streams, Free Lossless Audio codec (FLAC), Audio data compressed using the MPEG audio encoding scheme, Realmedia Audio data, Vorbis audio data, Windows Media Audio, and more.

@section media_video Video Playback

The following video stream formats are recommended for use with EGT:

Format           | Processor Support
---------------- | -----------------
Uncompressed YUV | All
MPEG1            | All
MPEG2            | All
MPEG4            | All
H.264            | SAMA5D4
VP8              | SAMA5D4
VP9              | SAMA5D4

Playing video in EGT is usually done with the egt::VideoWindow widget.

@snippet "../examples/snippets/snippets.cpp" video1


@section media_camera Camera Capture and Playback

EGT makes direct and indirect advantage of the [Video4Linux2](https://linuxtv.org/docs.php) (V4L2)
device drivers and API for supporting realtime video capture on Linux systems.
It supports many USB webcams, TV tuners, and related devices, standardizing
their output, so programmers can easily add video support to their applications.

Video4Linux2 provides access to the [Image Sensor Controller](https://www.linux4sam.org/bin/view/Linux4SAM/UsingISC) (ISC) and [Image Sensor Interface](https://www.linux4sam.org/bin/view/Linux4SAM/UsingIsi6) (ISI) peripherals available on some Microchip processors.

@imageSize{v4l2.png,width:300px;}
@image html v4l2.png "Video4Linux2 API"
@image latex v4l2.png "Video4Linux2 API" width=6cm

To capture any V4L2 camera to a file, the egt::v1::experimental::CameraCapture
class can be used.

@snippet "../examples/snippets/snippets.cpp" capture0

To capture and display a live V4L2 camera video feed, the egt::v1::CameraWindow
widget can be used.

@snippet "../examples/snippets/snippets.cpp" camera0

@section media_audio Audio Playback

Playing audio in EGT is usually done with the egt::AudioPlayer class.  This
class supports all of the expected features for playing various audio formats
such as random seeking and pausing.

@snippet "../examples/snippets/snippets.cpp" audio1

If a sound needs to be quickly played asynchronously, for example in response to
a button press, the egt::v1::experimental::Sound class is usually more
appropriate.

@snippet "../examples/snippets/snippets.cpp" sound1
