@page started Getting Started

[TOC]

There are various ways to get started with EGT depending on your requirements.
If you are building EGT on a PC, you can start by just fetching the source of
the EGT project.  If you are developing for and deploying to a target board,
the easiest way to work with EGT is part of a
[buildroot](https://www.linux4sam.org/bin/view/Linux4SAM/BuildRoot)
or [Yocto Project](https://www.linux4sam.org/bin/view/Linux4SAM/YoctoProject)
SDK which already includes a cross compiler and all of the necessary
dependencies and tools.

See @subpage eclipse for how to setup the Eclipse IDE for host or cross compiling.

If you are planning to work on EGT itself, see the
<a href="https://github.com/linux4sam/egt/blob/master/CONTRIBUTING.md">CONTRIBUTING.md</a>
file in the root of the EGT repository.

@section started_fetching Fetching Source

Fetch the source:

@code{.unparsed}
git clone --recursive https://github.com/linux4sam/egt.git
@endcode

@note Note the `--recursive` option.  This option is necessary to initialize any
git submodules in the project.  If you forget to do this, autogen.sh will do it
for you.

@section started_building Building

EGT is tested on Ubuntu 18.04, but is also known to work on Ubuntu 16.04 as well
as other distributions like Debian and Fedora.  These instructions should be
adjusted accordingly.

EGT uses the GNU Autotools, like
[automake](https://www.gnu.org/software/automake/manual/html_node/index.html),
for building on Linux platforms.

To build for a PC, first install the required dependencies for building EGT:

@code{.unparsed}
sudo apt install build-essential automake autoconf libtool pkg-config \
    libdrm-dev libinput-dev libcairo-dev libjpeg-dev libmagic-dev gettext
@endcode

Optional, but recommended, @ref intro_deps include:

@code{.unparsed}
sudo apt install librsvg2-dev liblua5.3-dev libcurl4-openssl-dev \
     libxkbcommon-dev xkb-data
sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
sudo apt install libplplot-dev plplot-driver-cairo
sudo apt install libasound2-dev libsndfile1-dev
@endcode

@note On Ubuntu 16.04 the package plplot12-driver-cairo should be used instead
of plplot-driver-cairo.

Then, clone the source and initialize Autotools.  Typically Autotools only needs
to be initialized once and it will automatically pick up most changes.  However,
when making changes to the configure scripts, the environment, or makefiles
re-running autogen.sh, or more simply `autoreconf -fvi`, is necessary.

@code{.unparsed}
git clone --recursive https://github.com/linux4sam/egt.git
cd egt
./autogen.sh
@endcode

Then, configure and build.

@code{.unparsed}
./configure
make
@endcode

You can also build out of the source tree.  This is useful, for example, to use
the same code tree with different parameters and environments to configure the
build in as many different build directories as you wish.  It also avoids
polluting the source tree with build artifacts.

@code{.unparsed}
mkdir -p build/host
cd build/host
../../configure
make
@endcode

Then, run an example.

@code{.unparsed}
cd examples/basic
./basic
@endcode

EGT also supports cross compilation for target systems. If you already have a
cross compiler available with dependent libraries available, add it to your
PATH and follow the standard instructions.

@note When adding cross compilation paths to the PATH environment variable, it
should come before any existing system paths.

@code{.unparsed}
./autogen.sh
./configure --prefix=/nfsroot/root/egt/ --host=arm-buildroot-linux-gnueabi
make
@endcode

@section started_configure Configure Options

You can run "./configure --help" to see more options.  Some non-standard
options are listed here, but not all.

@par `--enable-code-coverage`
Whether to enable code coverage support

@par `--enable-sanitize`
turn on sanitize analysis tools

@par `--enable-debug`
enable debugging support [default=yes]

@par `--enable-gcov`
turn on code coverage analysis tools

@par `--enable-profile`
turn on profiling tools

@par `--disable-icons`
do not install icons [default=no]

@par `--enable-examples`
build examples [default=yes]

@par `--disable-virtualkeyboard`
include support for virtualkeyboard [default=yes]

@par `--enable-lto`
enable gcc's LTO [default=no]

@par `--enable-lua-bindings`
enable lua bindings (default: no)

@par `--enable-simd`
build with simd support [default=no]

@par `--enable-snippets`
build snippets examples [default=no]


@section started_buildroot Cross Compiling With Buildroot

EGT is included as a package as part of the
[buildroot-external-microchip](https://github.com/linux4sam/buildroot-external-microchip)
project. To build a complete SDK and target filesystem for a board, follow the
instructions found in that project's
<a href="https://github.com/linux4sam/buildroot-external-microchip/blob/master/README.md">README</a>.

See the configs directory in the buildroot-external-microchip project for
a graphics defconfig for your processor and board.


@section started_yocto Cross Compiling With Yocto

EGT is included as a package as part of the
[meta-atmel](https://github.com/linux4sam/meta-atmel) Yocto layer project. To
build a complete SDK and target filesystem for a board, follow the
instructions found in that project's
[README](https://github.com/linux4sam/meta-atmel/blob/warrior/README).

@section started_windows Building for Windows

The EGT examples can be built for 32 bit or 64 bit Windows on Linux using the
[mxe project](https://mxe.cc/).

For example, to build 64 bit static binaries, this can be done with the
`x86_64-w64-mingw32.static` target.

@code{.unparsed}
git clone https://github.com/mxe/mxe.git
cd mxe
make MXE_TARGETS=x86_64-w64-mingw32.static cc libpng jpeg freetype cairo libsndfile sdl2 librsvg
... wait ...
export PATH=`pwd`/usr/bin:$PATH
@endcode

@code{.unparsed}
cd ~/egt
./configure --host=i686-w64-mingw32.static
make
@endcode

@section started_first First Application

The *examples* folder of the EGT project contains various examples.  This is the
basic example which shows the minimum necessary to create a window with a button
on it.

The include file `<egt/ui>` contains most basic functionality needed.

@snippet "../examples/basic/basic.cpp" Example

@imageSize{basic.png,width:400;height:auto;}
@image html basic.png "Basic Example"
@image latex basic.png "Basic Example"

If you want to use a UI file instead.

@snippet "../examples/basicui/basicui.cpp" Example


@section started_zeal Using this Documentation Offline or With Other Development Tools

This documentation is also available in the form of a docset that can be
read and searched using a tool like [Dash](https://kapeli.com/dash) (macOS) and
[Zeal](https://zealdocs.org/) (macOS/Windows/Linux).  In turn, the documentation
can be integrated into an IDE or tools like Atom/VSCode for an indexed and
easily searchable API reference.  These tools also provide a method for
automatically or manually updating to the latest EGT documentation.

Once you have one of these tools installed, the docset feed URL is located at:

https://linux4sam.github.io/egt-docs/docset/EGT.xml

@note To install Zeal, see the [official documentation to download and
    installing](https://zealdocs.org/download.html).  Specifically, on Ubuntu,
    it is recommended to get the latest version with their PPA.
    @code{.unparsed}
    sudo add-apt-repository ppa:zeal-developers/ppa
    sudo apt update
    sudo apt install zeal
    @endcode
