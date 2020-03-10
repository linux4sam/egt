 @page topics Developer Topics

This chapter discusses some common problems and advanced developer topics.

@section topics_black Black Screen

If you see a black screen when you run your application, make sure you call
egt::v1::Window::show() at least once somewhere in your application.

@section topics_threads Multiple Threads and Thread Safety

The EGT API is not thread safe, but there are well defined ways provided and
supported to use the EGT API in a multi-threaded application by following a
couple simple rules.  The thread the main egt::EventLoop is started on, usually
through egt::v1::Application::run(), must not be directly accessed by a separate
thread.  This means any widgets or windows that are managed by that main thread
should not be directly accessed by another thread.

Instead, EGT defines a solution to easily use multiple threads and interact with
the API in a thread safe manner through the event loop and Asio.  To call a
function in the EGT API from another thread, you must egt::asio::post() or
egt::asio::dispatch() a call to the EventLoop.  This should not be confused with
messaging.  Calling egt::asio::post() or egt::asio::dispatch() is like calling a
function.  However, based on which variation you use it will either directly or
indirectly happen in a thread safe manner.

For example, to change the position of a button from another thread, the
following can be done.

@code{.cpp}
egt::asio::post(egt::Application::instance().event().io(), [&button](){ button.x(5); });
@endcode

To run any code you want from another thread on the EventLoop thread, post() a
lamba or function.

@code{.cpp}
egt::asio::post(egt::Application::instance().event().io(), []() {
        ...
    });
@endcode

@section topics_lifetime Widget Lifetime

There are various different forms of managing widget lifetime.  They can be
managed explicitly on the stack, and they can also be managed on the heap,
usually through C++ smart pointers.

Everything is on the stack.

@code{.cpp}
int main(int argc, const char** argv)
{
    egt::Application app(argc, argv);

    egt::TopWindow window;
    egt::Button button(window, "Press Me");
    button.align(egt::AlignFlag::center);
    window.show();

    return app.run();
}
@endcode

Have EGT manage the lifetime of widgets.

@code{.cpp}
void add_buttons_to_window(egt::Window& win)
{
    win.add(make_shared<egt::Button>("button 1", egt::Rect(egt::Point(), egt::Size(100, 40))));
    win.add(make_shared<egt::Button>("button 2"));
}

int main(int argc, const char** argv)
{
    egt::TopWindow win;
    ...
    add_buttons_to_window(win);
    ...
    return app.run()
}
@endcode

Keep widgets around based on the scope of its owning object.

@code{.cpp}
class MyWindow : public egt::TopWindow
{
public:
    explicit MyWindow(const Size& size)
        : egt::TopWindow(size),
          m_grid(Tuple(2, 2))
    {
        add(m_grid);
    }

protected:
    egt::StaticGrid m_grid;
};
@endcode

@section topics_networking Networking and Serial Communications

@see @ref events_loop

The underlying EventLoop implementation in EGT uses Asio, which you can directly
access and is part of the full EGT API.  Asio provides an excellent asynchronous
networking implementation that you get for free with EGT. It can provide
efficient non-blocking I/O to service thousands of connections at once. The Asio
transport handles all aspects of networking (DNS lookups, creating TCP sockets,
connecting and listening, etc). It supports IPv4 and IPv6.  It supports both
plain and TLS secured sockets when the appropriate libraries are available.

The same is true for reading and writing to several other interfaces, including
serial ports using the asio::serial_port object.

A benefit of using Asio for networking is it it automatically built into the EGT
event loop automatically.

Outside of that, EGT provides some utility classes like
egt::v1::experimental::HttpClientRequest that uses libcurl underneath for full
HTTP/HTTPS support, and handles the integration of libcurl with Asio.

@section topics_compiler Compiler Setup and Options

The primary compiler used is gcc.  Other compilers like clang and mingw are a
work in progress and considered experimetal.  It is worth going over some common
compiler flags used when compiling EGT and EGT applications.

- `-std=c++14` - EGT requires C++14.
- `-fvisibility=hidden` - EGT uses explicit public visibility so everything else should be hidden by default.  gcc is particularly chatty about exporting symbols ina  shared library.
- `-fvisibility-inlines-hidden` - Same as `-fvisibility=hidden`
- `-fno-rtti` - EGT does not use runtime type information, like dynamic casting.
- `-fno-math-errno` - EGT does not check errno after calling math functions.  That can result int standard math functions being inlined.
- `-fmerge-all-constants` - EGT doesn't compare pointers of constants so it's perfectly fine to merge constants with the same values.
- `-ftree-vectorize` - In some cases you may want to explicitly list this, but it's usually automatic.
- `-Ofast` - In most cases, this gives the best performance compiler optimization wise.  `-O2` and `-O3` are not far off in most cases.  Just be aware that `-Ofast` uses some non-standard optimizations related to math.

@section topics_optimizing Finding Optimal Performance

EGT provides several `configure` options that can be used to help improve
performance based on your needs.  These options will automatically setup the
appropriate compiler and linker flags.

- `--disable-debug`
- `--enable-lto`
- `--disable-shared`

See @subpage started_configure for more information on available configure options.

@section topics_thirdparty Using Third Party Libraries with the EGT Event Loop

EGT has its own event loop, but sometimes when integrating third party libraries
those libraries also have their own idea of an event loop.  There are at least 4
different ways to integrate third party libraries and software with the EGT
event loop:

-# Attach a file descriptor to the EGT event loop.
-# Using an egt::PeriodicTimer to run the third party library.
-# Instead of using egt::v1::Application::run(), implement your own loop.
-# Spawn separate threads and use egt::asio::post() and egt::asio::dispatch().

@subsection topics_thirdparty_fd Attach a file descriptor to the EGT event loop

Some third party libraries open a resource that is identified with a file
descriptor.  In most cases, you can get this file descriptor and provide it to
the EGT event loop to have it monitor for when some action needs to be taken on
the file descriptor - either dierctly or still using the third party library.  As
long as the third party library does not block, this is usually the best option
even though it can require a little more setup.

The @b gpio example does this by attaching a gpioevent_request file descriptor
into the event loop so that any read events that happen on the file descriptor
can be seamlessly handled.

@snippet "../examples/gpio/gpio.cpp" ExampleFd2Event

EGT uses this method internally for several input backends like tslib and
libinput.  You can view the the source code of the associated
egt::detail::InputTsLib and egt::detail::InputLibInput classes for examples of
how to do this.  Both of those classes use the egt::asio::posix::stream_descriptor
class for wrapping the descriptor.

@subsection topics_thirdparty_timer Using an egt::PeriodicTimer to run the third party library

If the third party library provides the ability to poll or step to do processing
at regular intervals, it can make sense to simply create an egt::PeriodicTimer
and have it routinely call the third party library.

@code{.cpp}
int main()
{
    egt::Application app;
    egt::TopWindow window;

    egt::PeriodicTimer timer(std::chrono::seconds(1));
    timer.on_timeout([]()
    {
        // call third party library
    });
    timer.start();

    window.show();

    return app.run();
}
@endcode

@subsection topics_thirdparty_loop Instead of using egt::Application::run(), implement your own loop

Normally, you call Application::run() and that blocks forever until told to exit.
However, you may wish to implement your own loop in order to run the EGT event
loop and any other third party processing.

@code{.cpp}
int main()
{
    egt::Application app;

    while (true)
    {
        // call third party library

        // run one iteration of the EGT event loop
        app.event().step();
    }

    return 0;
}
@endcode

@subsection topics_thirdparty_threads Spawn separate threads

Spawn separate threads and use egt::asio::post() and egt::asio::dispatch() to
communicate between the threads safely.  Remember, EGT is not thread safe.  It
is never safe to assume it is ok to access any EGT object from multiple threads.
