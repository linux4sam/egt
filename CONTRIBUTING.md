![Microchip](docs/src/microchip_logo.png)

# Contributing

After you have setup your development machine and you can build EGT, you now have
everything you need to contribute to this project.  However, there are some
extra tools that are useful for development.

There are multiple ways of contributing to the future success of EGT.  For
example,  one could contribute on the documentation, code, dependencies, or file
bug reports.

## Code of Conduct

This project and everyone participating in it is governed by the
[EGT Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to
uphold this code of conduct.

## Development Tools Packages

There are some extra tools that you can install with your distribution package
manager.

```sh
sudo apt-get install cloc doxygen graphviz clang clang-tidy
```

## Extra Development Tools

We like to stay on a recent version of development tools, so while these tools
are probably packaged for your distribution, there is a chance they are old.
You should download and install the latest version of these tools manually.

- <a href="http://astyle.sourceforge.net/">Astyle</a>
- <a href="http://cppcheck.sourceforge.net/">cppcheck</a>
- Your favorite editor.

## General Coding Rules

There are some general guidelines, or rules, that must be followed.

- Prefer standard C++11 before anything else to solve any problem.  A compelling
reason is needed to diverge from this.
- Conditional build compilation, and specifically any dependency on config.h,
must not be in any public include file.
- Anything not part of the public API goes in the egt::detail namespace, and
additionally if it can be treated as a different compilation unit or header,
then it goes in the include/egt/detail/ or src/detail/ dir.
- Private headers not necessary to be in the public indlude folder should remain
in the src dir.
- As a general fule, no compiler warnings should be emitted in debug or release
builds.
- As a general rule, no cppcheck warnings should be emitted.

## Static Analysis

Static analysis is useful for detecting unexpectd behavior or bugs before they
happen.  The tool used by EGT is cppcheck to do this, but there are several other
tools available.

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

The C++ coding style of the EGT project mostly adhere's to the Allman style with
a couple exceptions.  This coding style is enforced throughout the codebase.

<a href="http://astyle.sourceforge.net/">Astyle</a> is used to enforce this
coding style with the *scripts/style.sh* script. This can be automatically
executed for all of the source code with a make target by typing:

```sh
make style
```

Be careful not to abuse this and change coding style just for the fun of it,
especially on source that you are not currentoly working on.

## Git Commit Messages

- Limit the first line to 72 characters or less
- Include a Signed-off-by: line near the end.

## Generating Documentation

This documentation is generated with Doxygen, which is managed by the EGT build
system.  To generate the documentation at any time run:

```sh
make docs
```

The resulting documentation will be in the *docs* folder.
