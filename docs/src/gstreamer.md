@page gstreamer GStreamer

[TOC]

EGT leverages GStreamer extensively for its multimedia features, simplifying user experience by automatically configuring GStreamer for common use cases.

@section gstreamer_basics GStreamer Basics

To get started, familiarilize yourself with GStreamer concepts through the [official tutorials](https://gstreamer.freedesktop.org/documentation/tutorials/index.html).

### Pipeline and Elements

GStreamer operates using pipelines composed of interconnected elements based on their capabilities. Typically, a source element reads the video, other elements can process it, and a sink element displays it.

### gst-launch-1.0

The `gst-launch-1.0` tool provided by GStreamer is useful for understanding and debugging pipelines.
This [tool page](https://gstreamer.freedesktop.org/documentation/tutorials/basic/gstreamer-tools.html#gstlaunch10) explains its usage and syntax pipeline description syntax.

For example, you can test video with the following command on a PC:
```
gst-launch-1.0 videotestsrc ! videoconvert ! autovideosink
```
On a board:
```
gst-launch-1.0 videotestsrc ! videoconvert ! kmssink force-modesetting=true
```

@section egt_implementation EGT Implementation

EGT offers several multimedia components:
- VideoWindow: a video player
- CameraWindow: a camera player
- CameraCapture: a camera recorder
- AudioPlayer: an audio player

Each component integrates an instance of `GstDecoderImpl`, the core of GStreamer support within EGT.

### Inputs

EGT utilizes two GStreamer elements to manage inputs.

#### uridecodebin

[uridecodebin](https://gstreamer.freedesktop.org/documentation/playback/uridecodebin.html) handles most inputs, including files and streams, by selecting a suitable source element and linking it to a `decodebin` element.

It is used by `VideoWindow` and `AudioPlayer`.

#### v4l2src

Special files as `/dev/video0` are not managed by `uridecodebin`, but [v4l2src](https://gstreamer.freedesktop.org/documentation/video4linux2/v4l2src.html).

It is used by `CameraWindow` and `CameraCapture`.

### Outputs

EGT supports several sink elements:
- `appsink` to display video
- `filesink` to record video
- `autoaudiosink` to play the audiotrack

While `filesink` and `autoaudiosink` usage is straightforward, the usage of `appsink` requires additional configuration from EGT.

#### appsink

The `appsink` element enables EGT to obtain frame buffers of the video. It is essential that `appsink` receives frames that matches the size and format of the EGT `Window`.

To ensure this, a `capsfilter` element is placed before the `appsink` element, specifying the required size and format and is achieved with the assistance of the `videoconvert` and `videoscale` elements. EGT then processes these frame buffers to update the display.

### Custom Pipeline

EGT automatically computes the pipeline description for the basic use cases it handles. To accommodate additional scenarios, a custom pipeline description can be provided to the `GstDecoderImpl` using the `custom_pipeline` method. When a custom pipeline is provided, EGT uses it as is, making it your responsibility to ensure the correct output for the `appsink` element.

Even with a custom pipeline, it is assumed that the output will be displayed in a `Window`. Therefore, the pipeline must include an `appsink` element named `appsink` and a `capsfilter` element named `vcaps`. If the `Window` is resized, EGT will reconfigure the `capsfilter` to match the new size.

Additionally, when using a custom pipeline, EGT assumes the presence of an audio track. To manage volume, the pipeline must include a `volume` element named `volume`.

#### Examples

- Video + Audio Display
```
egt_video --pipeline "uridecodebin uri=file:///usr/share/egt/examples/video/microchip_corporate_mpeg2.avi name=video ! videoconvert ! videoscale ! capsfilter name=vcaps caps=video/x-raw,width=320,height=192,format=I420 ! appsink name=appsink video. ! queue ! audioconvert ! volume name=volume ! autoaudiosink"
```
- Camera Display
```
egt_video --pipeline "v4l2src device=/dev/video0 ! videoconvert ! videoscale ! capsfilter name=vcaps caps=video/x-raw,width=320,height=192,format=I420 ! appsink name=appsink"
```
- Camera Display + Recording
```
egt_video --pipeline "v4l2src device=/dev/video0 ! tee name=source ! queue ! videoconvert ! videoscale ! video/x-raw,width=320,height=240 ! avimux ! filesink location=capture.avi source. ! queue ! videoconvert ! videoscale ! capsfilter name=vcaps caps=video/x-raw,width=320,height=192,format=I420 ! appsink name=appsink"
```
- Screen Mirroring
On the desktop machine (it requires X11):
```
gst-launch-1.0 -v ximagesrc  ! videoconvert  ! videoscale ! video/x-raw,format=I420,width=800,height=480,framerate=20/1 ! jpegenc ! rtpjpegpay ! udpsink host=10.159.245.199 port=5000
```
And on the board:
```
egt_video --width 800 --height 480 --pipeline "udpsrc port=5000 caps=application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! capsfilter name=vcaps caps=video/x-raw,width=800,height=480,format=I420 ! appsink name=appsink"
```

@section debugging Debugging

When multimedia is not functioning correctly, since EGT is essentially a wrapper around GStreamer, the first step is to determine whether the issue lies at the GStreamer level. This can be accomplished through a few diagnostic steps.

### gst-launch-1.0

The first step is to verify that the pipeline used by EGT, or the one you intend to use, functions correctly with GStreamer alone.

To obtain the pipeline description used by EGT, set the `EGT_DEBUG` environment variable to `2` or lower. This will generate an info debug line that indicates the pipeline description computed by EGT and provided to GStreamer.

For example, with the `egt_player` application on a board:
```
92954018 [info] GStreamer pipeline: uridecodebin uri=file:///usr/share/egt/examples/video/microchip_corporate_mpeg2.avi caps=video/x-raw(ANY) name=source ! videoconvert ! videoscale ! capsfilter name=vcaps caps=video/x-raw,width=320,height=192,format=I420 ! appsink name=appsink
```
The pipeline cannot be used directly with `gst-launch-1.0` due to the presence of the `appsink` element, which is designed for applications that handle video buffers. Instead, another sink must be used. Although this means the configuration will not be identical, it still helps verify that the rest of the pipeline functions correctly.

You can use `kmssink` with `force-modesetting=true` to utilize the base layer of the LCD. Ensure that you specify a pixel format supported by the base layer. The frame size must match the native size of the base layer. If you require a specific size, you will need to use the `render-rectangle` parameter.

Here is an example of a basic approach:
```
gst-launch-1.0 uridecodebin uri=file:/usr/share/egt/examples/video/microchip_corporate_mpeg2.avi caps="video/x-raw(ANY)" name=source ! videoconvert ! videoscale ! capsfilter name=vcaps caps=video/x-raw,format=RGB16 ! kmssink force-modesetting=true

```
To more closely match the EGT configuration, you can use the high-end overlay with the same size and position:
```
gst-launch-1.0 uridecodebin uri=file:/usr/share/egt/examples/video/microchip_corporate_mpeg2.avi caps="video/x-raw(ANY)" name=source ! videoconvert ! videoscale ! capsfilter name=vcaps caps=video/x-raw,width=320,height=192,format=I420 ! kmssink plane-id=43 render-rectangle="<240, 144, 320, 192>"
```
You can retrieve the `plane-id` using the `modetest` command.

If you do not have a working pipeline with `gst-launch-1.0`, it is unlikely to work with EGT. Therefore, you must first resolve any issues at the GStreamer level or at a lower level, such as kernel drivers.

### GStreamer Debug Logs

Another tool that can be utilized to diagnose issues is the GStreamer debug logs, which are controlled by the `GST_DEBUG` environment variable.

Typically, setting this variable to `4` provides sufficient information to identify problems. It is important to note that errors may not always be reported in the error-level logs; they can also appear in the info-level logs.

For more detailed information about debug logs, please refer to the [GStreamer documentation](https://gstreamer.freedesktop.org/documentation/tutorials/basic/debugging-tools.html).

### Pipeline Graphs

It's possible to obtain a visual representation of the pipeline computed by GStreamer from the provided description. This can be useful for verifying the configuration of the elements.

This feature is available with both `gst-launch-1.0` and EGT. To enable it, set the `GST_DEBUG_DUMP_DOT_DIR` environment variable to the directory where you want to store the graphs.

For example:
```
$ GST_DEBUG_DUMP_DOT_DIR=./logs/ egt_video -i file:///usr/share/egt/examples/video/microchip_corporate_mpeg2.avi
$ ls logs
0.00.01.878953756-gst-discoverer-target-state.dot  0.00.02.213577756-egt.READY_PAUSED.dot
0.00.02.035164586-egt.NULL_READY.dot		   0.00.02.245329073-egt.PAUSED_PLAYING.dot
```
These files correspond to the pipeline graph at various pipeline state transitions, such as when the state changes from paused to playing. You can use the `xdot` software as a viewer for these dot files to visualize the pipeline.

@imageSize{xdot_example.png,width:1000px;}
@image html xdot_example.png "xdot example"
@image latex xdot_example.png "xdot example" width=15cm
