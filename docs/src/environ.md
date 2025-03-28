 @page environ Environment Variables

This chapter discusses several environment variables that can be used to
configure the EGT library at run time.

EGT will look at several run time environment variables to change the operation
of the library.  For example, you can change some configurations and set log
levels using environment variables.

There are many ways to set an environment variable for a process, but generally
speaking you can run a process in a shell and specify some environment
variables just for that process:

@code{.sh}
EGT_DEBUG=1 EGT_KMS_BUFFERS=1 ./egt_space
@endcode

<dl>
  <dt>EGT_DEBUG</dt>
  <dd>
    Set the numeric log level for the framework. If the library is compiled with
    debug turned off, the internal DEBUG and TRACE log levels are removed at
    compile time.
    - TRACE = 0
    - DEBUG = 1
    - INFO = 2
    - WARN = 3
    - ERROR = 4
    - CRITICAL = 5
    - OFF = 6

    See @subpage started_configure for more information on compile time options.

  </dd>

  <dt>EGT_BACKEND</dt>
  <dd>
    Select what backend to use for rendering to the screen.  If this environment
    variable is not specified, a suitable default will be chosen.
    - kms
    - x11
    - sdl2
    - none
  </dd>

  <dt>EGT_KMS_BUFFERS</dt>
  <dd>
    Specify the number of buffers to use for KMS.  If you specify a single
    buffer, no buffering will be used.  If you specify 2 buffers, double
    buffering will be used.  If you specify 3 buffers, triple buffering will be
    used, and so on.
  </dd>

  <dt>EGT_INPUT_DEVICES</dt>
  <dd>
    Configure mapping of input devices to their EGT input backend.

    Available backends include:
    - libinput (default)
    - tslib
    - evdev

    Specifying input devices remove the default backend.

    Use libinput and tslib to handle different touchscreens:
    @code{.sh}
    EGT_INPUT_DEVICES=libinput:/dev/input/touchscreen0,/dev/input/touchscreen1;tslib:/dev/input/touchscreen2
    @endcode

    Get keyboard events for the USER button on a SOM1-EK board:
    @code{.sh}
    EGT_INPUT_DEVICES=evdev:/dev/input/event2
    @endcode
  </dd>

  <dt>EGT_ICONS_DIRECTORY</dt>
  <dd>
    Change EGT installed default icons directory with an absolute or relative path.

    The directory path contents specified must be in the expected format as the
    default EGT icons directory, containing subdirectories of icon size.  For
    example, 16px, 32px, 64px, and 128px.

    @b Example
    @code{.sh}
    EGT_ICONS_DIRECTORY=/usr/share/icons
    @endcode
  </dd>

  <dt>EGT_SCREEN_SIZE</dt>
  <dd>
    Set a custom screen size.  This is only possible with some backends, like X11.

    @b Example
    @code{.sh}
    EGT_SCREEN_SIZE=640x480 ./widgets
    @endcode
  </dd>

  <dt>EGT_SEARCH_PATH</dt>
  <dd>
    Add additional search directories to find resources.

    @b Example
    @code{.sh}
    EGT_SEARCH_PATH=/opt/icons:/usr/share/planes
    @endcode
  </dd>

  <dt>EGT_SCREEN_ASYNC_FLIP</dt>
  <dd>
    A non-empty value tells the screen backend to perform asynchronous flip
    operations.  Note that when this is enabled, screen tearing may be possible.

    @b Example
    @code{.sh}
    EGT_SCREEN_ASYNC_FLIP=1 ./widgets
    @endcode
  </dd>

  <dt>EGT_X11_NODECORATION</dt>
  <dd>
    A non-empty value turns off window decorations on an X11 window.
  </dd>

  <dt>EGT_TIME_DRAW</dt>
  <dd>
    When non-empty, print timing information for drawing every widget.
  </dd>

  <dt>EGT_TIME_EVENTLOOP</dt>
  <dd>
    When non-empty, print timing information for the event loop.
  </dd>

  <dt>EGT_SHOW_FPS</dt>
  <dd>
    When non-empty, print the frames per second of the event loop.
  </dd>

  <dt>EGT_NO_COMPOSITION_BUFFER</dt>
  <dd>
    Instead of using a composition buffer, always render directly into the
    framebuffer. This only works if there is a backend with a single
    framebuffer.  For example, when using KMS, the environment variable
    EGT_KMS_BUFFERS must equal 1.  Otherwise it has no effect and may not apply to
    all backends.
  </dd>

  <dt>EGT_WIREFRAME_ENABLE</dt>
  <dd>
    A non-empty value enables drawing of damage rectangles to the display.  This
    can be used to identify, visually, what is being redrawn each flip to the
    screen.  This is a debug option and can create additional burden on
    performance.  This option requires a composition buffer, so it cannot be
    used with EGT_NO_COMPOSITION_BUFFER.
  </dd>

  <dt>EGT_WIREFRAME_DECAY</dt>
  <dd>
    When non-zero, specifies the amount of decay in milliseconds to visually
    keep wireframe rectangles around.  This option only applies when
    EGT_WIREFRAME_ENABLE is non-empty.
  </dd>

  <dt>EGT_SHOW_SCREEN_BANDWIDTH</dt>
  <dd>
    When non-empty, logs the bandwidth to render the final screen frames.  In
    other words, the number of bytes transmitted through damage rectangles to
    the framebuffer being displayed. This can be used to gauge the amount of
    data changing to render a screen.
  </dd>

  <dt>EGT_LIBINPUT_VERBOSE</dt>
  <dd>
    When non-empty, turns on verbose logging from libinput as log level info.
  </dd>

  <dt>EGT_TIME_INPUT</dt>
  <dd>
    When non-empty, prints timing information for handling input events.
  </dd>

</dl>
