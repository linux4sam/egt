![Microchip](docs/src/microchip_logo.png)

# Contributing

There are multiple ways of contributing to the future success of EGT.  For
example,  one could contribute on the documentation, code, dependencies, or file
bug reports.  After you have setup your development machine and you can build
EGT, you now have everything you need to get started contributing.  However,
in some cases there are recommended or necessary extra tools that can help.

## Code of Conduct

This project and everyone participating in it is governed by the
[EGT Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to
uphold this code of conduct.

## Development Tools Packages

There are some extra tools that you can install with your distribution package
manager.

```sh
sudo apt install cloc doxygen graphviz clang clang-tidy aspell
```

## Extra Development Tools

We like to stay on a recent version of development tools, so while these tools
are probably packaged for your distribution, there is a chance they are old.
You should download and install the latest version of these tools manually.

- [Astyle](http://astyle.sourceforge.net/)
- [cppcheck](http://cppcheck.sourceforge.net/)
- Your favorite editor: [Emacs](https://www.gnu.org/software/emacs/).

## General Coding Rules

There are some general guidelines, or rules, that must be followed.

- Prefer standard C++11 before anything else to solve any problem.  A compelling
reason is needed to diverge from this.
- Conditional build compilation, and specifically any dependency on config.h,
must not be in any public include file.
- Anything not part of the public API goes in the egt::detail namespace, and
additionally if it can be treated as a different compilation unit or header,
then it goes in the include/egt/detail/ or src/detail/ dir.
- Headers that do not need to be in the public include folder should
remain in the src dir.
- As a general rule, no compiler warnings should be emitted in debug or release
builds.
- As a general rule, no cppcheck warnings should be emitted.

## Static Analysis

Static analysis is useful for detecting unexpected behavior or bugs before they
happen.  The tool used by EGT is cppcheck to do this, but there are several
other tools available.

```sh
make cppcheck
```

If any warnings are emitted by cppcheck, they should be fixed or judiciously
silenced with the cppcheck comment syntax.

```cpp
// cppcheck-suppress unreadVariable
static std::thread t([]()
{
    GMainLoop* mainloop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainloop);
});
```

## Coding Style

The C++ coding style of the EGT project mostly adheres to the Allman style with
a couple exceptions.  This coding style is enforced throughout the codebase.

- Indententation is 4 spaces.  No tabs.
- All type names including classes, enums, and using/typedef declarations use
CamelCaseNaming and begin with a capital letter.
- All functions and variables including member functions, member variables, and
local variables use lowercase names with words_seperated_by_an_underscore.
- Prefer not to use postfix '_t' on type names as this _namespace_ is
considered reserved.

### Automatic Code Style

[Astyle](http://astyle.sourceforge.net/) is used to enforce this
coding style with the *scripts/style.sh* script. This can be automatically
executed for all of the source code with a make target by typing:

```sh
make style
```

Be careful not to abuse this and change coding style just for the fun of it,
especially on source that you are not currently working on.

## Assertions vs. Exceptions

- Use asserts to check for errors that should never occur. Use exceptions to
  check for errors that might occur, for example, errors in input validation on
  parameters of public functions.
- Use exceptions when the code that handles the error might be separated from
  the code that detects the error by one or more intervening function calls.

## Git Commit Messages

- Limit the first line to 72 characters or less
- Include a Signed-off-by: line near the end.

## Generating LUA Bindings

[SWIG Documentation](http://www.swig.org/Doc4.0/SWIGDocumentation.html)

swig @b 4.0 or later should be used, mostly to pickup some improved support for
C++11.  This means you may need to download the swig release tarball and build
it because the package not be available.

## Generating Documentation

This documentation is generated with Doxygen, which is managed by the EGT build
system.  The recommended version of doxygen is @b 1.8.17.  If other versions are
used, ther are sometimes noticeable differences.

To generate the documentation at any time run:

Generate HTML:
```sh
make docs
```

Generate docset:

Docset generation depends on a python tool called doxytag2zealdb.  It can be
installed using pip:
```sh
pip install doxytag2zealdb
```

```sh
make docset
```

Generate PDF:
```sh
make docs
cd docs/latex && make
```

The resulting documentation will be in the *docs* folder.

## Build/Generation Testing

When making code or documentation changes and preparing commits, there are many
different ways to build and test.  Here is an example of sort of a routine to
run through and correct any errors/warnings seen.

```sh
make maintainer-clean
./autogen.sh
./configure
make
make spellcheck
make style
make docs
make checkheaders
make cppcheck
make tidy
make distcheck
```

Running make distcheck in a cross compile environment takes a little more
effort.  For example:

```sh
make distcheck \
    DISTCHECK_CONFIGURE_FLAGS="--host=arm-buildroot-linux-gnueabihf"
```

## clang

```sh
CC=clang-6.0 CXX=clang++-6.0 ./configure
```