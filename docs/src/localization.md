 @page local Internationalization and Localization

This chapter discusses how to make use of internationalization (i18n) and
localization support in EGT.

@section local_encoding UTF-8 Encoding

UTF-8 encoding is supported everywhere in EGT.  All text is stored in
std::string.  No other special storage classes are used.  All std::string
objects are assumed to be UTF-8 encoded within EGT.  For more information on why
this is done, see the [UTF-8 Everywhere Manifesto](http://utf8everywhere.org/).

To help understand details about Unicode and different character sets, see
[The Absolute Minimum Every Software Developer Absolutely, Positively Must Know About Unicode and Character Sets (No Excuses!)](https://www.joelonsoftware.com/2003/10/08/the-absolute-minimum-every-software-developer-absolutely-positively-must-know-about-unicode-and-character-sets-no-excuses).

@section local_gettext Text Translations

Any framework that supports language translations, otherwise known as
internationalization or i18n for short, should work without any issue with EGT.
This is largely an application level exercise as long as the method supports
UTF-8 encoding, or the conversion to and from UTF-8 encoding, as required by
EGT. However, the recommended method to perform text translations in an EGT
application, for example to display different languages, is using the mature and
well established
[GNU gettext](https://www.gnu.org/software/gettext/manual/index.html)
support.

> GNU `gettext` offers to programmers, translators, and even users, a well
> integrated set of tools and documentation. Specifically, the GNU `gettext'
> utilities are a set of tools that provide a framework to help other GNU
> packages produce multi-lingual messages. These tools include a set of
> conventions about how programs should be written to support message catalogs,
> a directory and file naming organization for the message catalogs themselves,
> a runtime library supporting the retrieval of translated messages, and a few
> stand-alone programs to massage in various ways the sets of translatable
> strings, or already translated strings.

Basically, gettext provides the ability to write all of your strings in one
language (usually English) in code, and then allow a "database" of different
translations for those strings to be loaded by the application depending on
language and locale settings at runtime.  In addition, gettext defines a
workflow and tools for creating those translation "databases."

@section local_gettext_example Example GNU gettext Steps

Here are the steps necessary to implement the usage of gettext in an
EGT application:

Create a .po (Portable Object) file:
@code{.sh}
make build-po
@endcode

If new strings are added, update the existing .po file:
@code{.sh}
make build-merge-po
@endcode

Use a text editor to open the .po file and add translations to all the strings.
You may also need to set the `charset` as UTF-8:
@code{.sh}
vi es.po

"Content-Type: text/plain; charset=UTF-8\n"

msgid "Hello World!"
mgsstr "Hola Mundo!"
@endcode

Generate a .mo (machine readable) file from .po:
@code{.sh}
make
@endcode

Set the LANGUAGE environment variable and run:
@code{.sh}
LANGUAGE=es.utf8 ./i18n
@endcode

@note In buildroot, you have to enable the locales you want to add a new text
domain for:

    Symbol: BR2_GENERATE_LOCALE [=en_US es_MX]
