@page debug Debugging

[TOC]

This chapter covers some of the many tools and techniques to debug an EGT
application or EGT itself.

@section debug_environ Environment Variables

See @subpage environ for information about environment variables that may be
useful for debugging.  Specifically, configuring logging internal to EGT.

@section debug_gdb GDB Tips

Outside of typical debugging with GDB, there are a couple cases where
specifically instructing GDB to break before an event occurs is useful.

EGT uses runtime asserts internally.  Be sure to compile the library without
-DNDEBUG and then you can break on any assert using GDB to examine the callstack
and the state of the application to figure out what happened leading up to the
assert.  EGT will compile with asserts enabled by default. See
@subpage started_configure for more information on compile time options.

Issue the following in a GDB session to break on an assert before the program
aborts:

@code
break __assert_fail
r
...assert...
bt
@endcode

@note This may be libc [implementation specific](https://refspecs.linuxbase.org/LSB_3.1.1/LSB-Core-generic/LSB-Core-generic/baselib---assert-fail-1.html).

Another technique EGT uses to expose non-fatal runtime errors is through the use
of C++ exceptions.  Like runtime asserts, it is often useful to understand the
callstack and what happened leading up to the exception being thrown.  By the
time the exception is caught, it is usually too late to get this information.

To catch when a C++ exception is thrown, before the stack starts unwinding, you
can issue the following in a GDB session:

@code
catch throw
r
...exception thrown...
bt
@endcode

@section debug_gdb_memory Using GDB on Low Memory Systems

EGT potentially links against many shared libraries and this can be a problem
interactively running gdb directly on a target board with limited memory.  The
safest approach is to go to a remote setup by using gdbserver.  However, there
are techniques that can be used to specifically prevent loading uninteresting
shared libraries.  In the following example, we prevent loading shared libraries.
Then, when we break, we load the shared libraries we care about and then we can
get a nice backtrace that makes sense.

@code
# gdb ./egt_video
GNU gdb (GDB) 8.1.1
Copyright (C) 2018 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "arm-buildroot-linux-gnueabihf".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./egt_video...done.
(gdb) handle SIGILL nostop
Signal        Stop      Print   Pass to program Description
SIGILL        No        Yes     Yes             Illegal instruction
.avi) starti /usr/share/egt/examples/video/wonder-park-trailer-1_h720p.mov.mpeg2
Starting program: /root/egt/bin/egt_video /usr/share/egt/examples/video/wonder-park-trailer-1_h720p.mov.mpeg2.aviProgram stopped.
0xb6fb6b40 in _start () from /lib/ld-linux-armhf.so.3
(gdb) set auto-solib-add 0
(gdb) c
Continuing.
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/libthread_db.so.1".Program received signal SIGILL, Illegal instruction.
[New Thread 0xb52802b0 (LWP 227)]
[New Thread 0xb49a42b0 (LWP 228)]
[New Thread 0xb41a32b0 (LWP 230)]
[New Thread 0xb371b2b0 (LWP 231)]
[New Thread 0xb2d322b0 (LWP 232)]
[New Thread 0xb25312b0 (LWP 233)]
[New Thread 0xb1bff2b0 (LWP 234)]
[New Thread 0xb13af2b0 (LWP 235)]
[New Thread 0xb0bae2b0 (LWP 236)]
[New Thread 0xb03282b0 (LWP 237)]
[New Thread 0xaddfa2b0 (LWP 238)]
[New Thread 0xad5bc2b0 (LWP 239)]Thread 3 "egt_video" received signal SIGSEGV, Segmentation fault.
[Switching to Thread 0xb49a42b0 (LWP 228)]
0xb6a5d104 in ?? () from /usr/lib/libplanes.so.1
(gdb) where
#0  0xb6a5d104 in ?? () from /usr/lib/libplanes.so.1
#1  0xb6ea1862 in ?? () from /root/egt/lib/libegt.so.1
Backtrace stopped: previous frame identical to this frame (corrupt stack?)
(gdb) shar planes
Reading symbols from /usr/lib/libplanes.so.1...done.
(gdb) shar egt
Reading symbols from /root/egt/lib/libegt.so.1...done.
(gdb) where
#0  0xb6a5d104 in plane_flip (plane=0xb49ff008, target=0) at plane.c:455
#1  0xb6ea1862 in egt::v1::detail::FlipJob::operator() (this=0xb1c57400)
    at ../../ui/src/detail/screen/kmsoverlay.cpp:34
#2  0xb6ea2450 in std::_Function_handler<void (), egt::v1::detail::FlipJob>::_M_invoke(std::_Any_data const&) (__functor=...)
    at /home/jhenderson/buildroot/sama5d27_som1_ek_egt/host/arm-buildroot-linux-gnueabihf/include/c++/7.4.0/bits/std_function.h:316
#3  0xb6df6b54 in std::function<void ()>::operator()() const (this=0xb49a3c28)
    at /home/jhenderson/buildroot/sama5d27_som1_ek_egt/host/arm-buildroot-linux-gnueabihf/include/c++/7.4.0/bits/std_function.h:706
#4  0xb6ea1662 in egt::v1::detail::FlipThread::run (this=0x602d8)
    at ../../ui/src/detail/screen/flipthread.h:54
#5  0xb6ea20b2 in std::__invoke_impl<void, void (egt::v1::detail::FlipThread::*)(), egt::v1::detail::FlipThread*> (
    __f=@0x87ba0: (void (egt::v1::detail::FlipThread::*)(egt::v1::detail::FlipThread * const)) 0xb6ea15a9 <egt::v1::detail::FlipThread::run()>,
    __t=@0x87b9c: 0x602d8)
    at /home/jhenderson/buildroot/sama5d27_som1_ek_egt/host/arm-buildroot-linux-gnueabihf/include/c++/7.4.0/bits/invoke.h:73
#6  0xb6ea195a in std::__invoke<void (egt::v1::detail::FlipThread::*)(), egt::v1::detail::FlipThread*> (
    __fn=@0x87ba0: (void (egt::v1::detail::FlipThread::*)(egt::v1::detail::FlipThread * const)) 0xb6ea15a9 <egt::v1::detail::FlipThread::run()>,
---Type <return> to continue, or q <return> to quit---
    __args#0=@0x87b9c: 0x602d8)
    at /home/jhenderson/buildroot/sama5d27_som1_ek_egt/host/arm-buildroot-linux-gnueabihf/include/c++/7.4.0/bits/invoke.h:95
#7  0xb6ea32ba in std::thread::_Invoker<std::tuple<void (egt::v1::detail::FlipThread::*)(), egt::v1::detail::FlipThread*> >::_M_invoke<0u, 1u> (this=0x87b9c)
    at /home/jhenderson/buildroot/sama5d27_som1_ek_egt/host/arm-buildroot-linux-gnueabihf/include/c++/7.4.0/thread:234
#8  0xb6ea327c in std::thread::_Invoker<std::tuple<void (egt::v1::detail::FlipThread::*)(), egt::v1::detail::FlipThread*> >::operator() (this=0x87b9c)
    at /home/jhenderson/buildroot/sama5d27_som1_ek_egt/host/arm-buildroot-linux-gnueabihf/include/c++/7.4.0/thread:243
#9  0xb6ea3264 in std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (egt::v1::detail::FlipThread::*)(), egt::v1::detail::FlipThread*> > >::_M_run
    (this=0x87b98)
    at /home/jhenderson/buildroot/sama5d27_som1_ek_egt/host/arm-buildroot-linux-gnueabihf/include/c++/7.4.0/thread:186
#10 0xb618d81a in ?? () from /usr/lib/libstdc++.so.6
Backtrace stopped: previous frame identical to this frame (corrupt stack?)
(gdb)
@endcode

@section debug_libtool Using GDB Without Installing Examples

Running GDB right in the build directory without having to install the examples
takes some special consideration.  By default, compiling the EGT library makes
use of a tool called [libtool](https://www.gnu.org/software/libtool/).

When libtool is used, it creates wrapper scripts that appear to be the
executable.  It does this so that any built libraries can be automatically found.
However, when you want to run GDB, you can't exactly run them on the wrapper
scripts.  However, libtool provides a way to do it.

@code
libtool --mode=execute gdb basic
@endcode

@section debug_libplanes Libplanes

@b libplanes has an environment variable that can be useful for dumping
information specifically about libplanes, which EGT can use for display setup
and kernel mode setting.  To see what libplanes has to say:

@code{.sh}
export LIBPLANES_DEBUG=1
@endcode

@section debug_cairo Debugging Cairo

Cairo comes with a nifty trace tool that will allow you to log any cairo library
calls to a file.  It works sort of like strace or ftrace, but more like a tape
recorder where you can record the cairo commands and then play them back.

@code{.sh}
sudo apt install cairo-perf-utils
@endcode

Then, run any application with the tool and it will log to a trace file in the
same directory by default.

@code{.sh}
cairo-trace ./basic
@endcode

Additionally, you can replay the trace against several backends to see which one
performs better.

@code{.sh}
cairo-perf-trace thermostat.26611.trace
@endcode

@section debug_tree Dump Widget Tree

In code, Application::dump() can be called to print the widget tree to an ostream.
However, the default application instance installs a signal handler to do the
same thing.  So, you can send the signal SIGUSR1 to an EGT process and it will
automatically print the widget tree.

@code{.sh}
killall -SIGUSR1 binary-name
@endcode

@note This is Linux only.

@section debug_screenshot Save Screenshot

In code, Application::paint_to_file() can be called to save a screenshot to a
PNG file. However, the default application instance installs a signal handler to
do the same thing.  So, you can send the signal SIGUSR2 to an EGT process and it
will automatically save a screenshot file.

@code{.sh}
killall -SIGUSR2 binary-name
@endcode

@note This is Linux only.
