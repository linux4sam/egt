@page events Events

[TOC]

This chapter discusses the event model and how events are handled.

@section events_loop Event Loop

The EventLoop is the inner loop of the framework. Its basic operation is
demonstrated with the following pseudo code:

@code{.cpp}
while (true)
{
    wait for events
    dispatch events
    draw
}
@endcode

It does this until the event loop is told to exit, and in turn, this usually
means the application exits.  For example, a call to egt::v1::EventLoop::quit()
would cause the event loop to exit.

The event loop in EGT is implemented with the [Asio](https://think-async.com/)
library.  Asio is a cross-platform C++ library for network and low-level I/O
programming that provides developers with a consistent asynchronous model using
a modern C++ approach.  Asio can be used as part of the boost or standalone, and
EGT comes with the standalone version built in. Asio provides far more features
than what is directly used by EGT itself.

@imageSize{asio.png,width:400px;}
@image html asio.png "Asio"
@image latex asio.png "Asio" width=8cm

EGT abstracts out any required direct involvement with Asio; however, should you
choose to use it directly, it is available for use in, for example, @ref
topics_networking "networking" and @ref topics_threads "multi-threaded"
applications. [Asio Documentation](http://think-async.com/Asio/asio-1.12.2/doc/index.html)
is a good reference point if you want to start using the Asio API directly.

@section events_prop Event Propagation

Every time a user touches the screen, clicks a button, or presses a key, an
event is generated and sent to the event loop and then dispatched to appropriate
handlers. Events can also come from other things like a window manager,
timers, or networking sockets.

Events are usually propagated from the top level widget down.  There are
exceptions to this; for example, when a widget grabs the pointer or keyboard,
and the natural top-down flow is short circuited.  For example, when a TextBox
has keyboard focus, it will be directly sent keyboard events without having to
navigate the widget tree.

@image html widget_hierarchy_event.png "Event"
@image latex widget_hierarchy_event.png "Event" width=8cm

@section events_data Event Data

Each event handler, like egt::v1::Widget::handle(), is called with an Event
object.  This object maintains the unique egt::EventId of the event, and any data
associated with the event, such as pointer or key event data.

@section events_handling Handling Events

When implementing a widget that needs to handle events, overriding the
egt::v1::Widget::handle() virtual method is the expected method to handle events.
However, when using an existing widget, it can be a burden to the subclass just
to handle events.  So, to handle events when using a widget, the
egt::v1::Widget::on_event() method can be used to register any number of callbacks. A
[lambda](https://en.cppreference.com/w/cpp/language/lambda) function may also be
used.

@code{.cpp}
egt::ImageButton settings("icon:cog.png");
settings.on_event([this](egt::Event& event)
{
    if (event.id() == egt::EventId::pointer_click)
    {
        if (m_popup.visible())
            m_popup.hide();
        else
            m_popup.show(true);
    }
});
@endcode

Another variation of the egt::v1::Widget::on_event() function allows for
specifying what events to filter as a second parameter.

@code{.cpp}
egt::ImageButton settings("icon:cog.png");
settings.on_event([this](egt::Event&)
{
    if (m_popup.visible())
        m_popup.hide();
    else
        m_popup.show(true);
}, {egt::EventId::pointer_click});
@endcode

egt::v1::Widget::on_event() can be called any number of times to register any
number of callbacks.

@section events_timers Timers

Timers are an intrinsic part of the EventLoop.  There are two main classes for
working with timers: Timer and PeriodicTimer.

Creating a timer is straightforward, and you can register any number of
callbacks with the egt::v1::Timer::on_timeout() function of a Timer or a
PeriodicTimer.

@code{.cpp}
egt::PeriodicTimer timer(std::chrono::seconds(1));

timer.on_timeout([]()
{
    cout << "timer fired" << endl;
});

timer.start();
@endcode

@section events_handling_extended Handling Extended or Custom Widget Events

The egt::EventId lists global events that don't necessarily originate in a
widget itself.  To handle custom or widget type specific events,
egt::v1::Signal member attributes can be added to a widget to provide custom
handles any user can connect to in order to receive the event.

The typical use case for egt::v1::Signal is the same as
egt::v1::Widget::on_event() except it can take any number of type of arguments,
including none.

@code{.cpp}
egt::CheckBox box;
box.on_checked_changed([&box]()
{
    std::cout << "CheckBox state is: " << box.checked() << std::endl;
});
@endcode
