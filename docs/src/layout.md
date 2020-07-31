@page layout Layout

[TOC]

This chapter discusses how Widget layout works.

Before diving into this, understanding @subpage geometry "Geometry"
semantics in EGT is necessary.

@section layout_box The Widget Box Model

@ref draw_loop first introduced the idea of the egt::Widget @b box.
Understanding the egt::Widget @b box is an important concept not only when it
comes to understanding how widgets are drawn, but also in understanding position
and layout. Each egt::Widget has a @b box accessed with egt::v1::Widget::box().
This is an egt::Rect that has an origin and a width and height.  If you call
egt::v1::Widget::move() or egt::v1::Widget::resize(), the box is changed.  When
drawing or performing layout, this is the box that is used or @a owned by the
egt::Widget.  By default, when an egt::Widget is told to draw, the egt::Painter
will be clipped to be within the widget's box.

Within the widget @b box, each widget also has a @b margin, @b border, and @b
padding width. These are intrinsic properties that are used when drawing a
widget.  How each of these properties applies visually is very similar to CSS
constructs and is demonstrated with the following diagram.

@image html layout.png
@image latex layout.png "Layout" width=8cm

In the case of a background color, it goes to the inside edge of the border.

@image html layout2.png
@image latex layout2.png "Layout Fill" width=8cm

The widget box itself has no width.  If any of the other properties like margin,
border, or padding are set to zero they have no width.

@section layout_intro Layout

There are two main ways to layout widgets: @b Fixed and @b Fluid.  These are not
mutually exclusive within the same application and can easily be mixed together.

@par Fixed
The first is absolute position and sizing.  This is limiting and usually less
preferred because it hard codes a layout to a specific display resolution and is
otherwise not fluid when manipulating the layout by adding or removing widgets
later during development or even dynamically at run-time.  This can easily be
cumbersome, but it's precise and works exactly as expected when desired.  The
only thing used to do position and size with this method is whatever the box()
of each widget is. This means that you must explicitly construct or set the size
and position of each widget manually with the proper size and position.
However, what tends to happen is the developer ends up doing the same
calculations that could otherwise be specified simpler with the fluid method.

@par Fluid
The second method is automatic, or fluid, positioning and sizing based on rules
that take into account various properties of widgets.  The result is the box()
of the widget is automatically changed, instead of being directly set. This
constraint-based approach to design allows you to build user interfaces that
dynamically respond to both internal and external changes.  It also goes without
saying, it also allows you to skip the sometimes tedious math in your head that
is usually involved with fixed positioning.

## Widget Alignment and Layout Properties

Each widget has an egt::v1::Widget::align() setting. Each widget also has an
egt::v1::Widget::min_size_hint() to bound the smallest size the widget can be.
If a widget has an alignment value of empty, usually the default, whatever size
it has will be used while still taking the egt::v1::Widget::min_size_hint() into
account. If the alignment value of the widget is empty, the size of the widget
will automatically be calculated based on several other properties, usually
based on some ratio like egt::v1::Widget::xratio(), egt::v1::Widget::yratio(),
egt::v1::Widget::horizontal_ratio(), or egt::v1::Widget::vertical_ratio() in
relation to the widget's parent.

## Layout Widgets

EGT provides several widgets like egt::v1::BoxSizer and egt::v1::StaticGrid to
help with automatic layout.  See @ref sizers for a complete list.

The layout algorithm used by sizers is based upon the idea of individual widgets
reporting their minimal required size and their ability to get stretched if the
size of the parent widget has changed. This will most often mean that the
programmer does not set the start-up size of a widget, the widget will rather be
assigned as a child to a sizer and this sizer will set the recommended size of
the widget. This sizer in turn will query its children (which can be other
widgets or even sizers) so that a hierarchy of sizers can be constructed.

### Sizers

The purpose of sizers is to help position widgets. They have a set of rules to
accomplish this and they are able to grow if needed. There are three types of
sizers with slightly different behaviors: the horizontal box sizer, the
vertical one, and the flex one. As a sizer will grow to contain all the child
widgets, you are responsible for it. It means there is no constraint for the
maximum size. Resizing of children only happen if you set some expand align
flags. Children will never be shrunk except in specific circumstances when
using the flex variant.

The horizontal and vertical box sizers behave in the same way but in different
directions. They grow automatically in the direction set as you add widgets.
Note that some logic doesn't make sense:
- Expanding a child if the sizer as a null size. Expanding allows to use the
space available, if the sizer as an empty size, there is no extra space
available.
- Setting the width of a horizontal box sizer or the height of a vertical box
sizer is useless as they grow automatically. It can be useful only if the
sizer needs to have a minimal size.
The children won't be resized except if you use the alignment expand flags.
Keep in mind that it doesn't make sense to expand a child if the sizer's size
is empty. It makes no sense to set the width of a horizontal box sizer or the
height of a vertical box sizer as they will grow to contain all the widgets.

The flex box sizer will try to position the widgets, on both horizontal and
vertical directions, within its current box. If the resulting layout is bigger
than the box, then it will grow as much as  needed. There is one case widgets
can be shrunk: if the width or the height of the widget is bigger than the ones
of the sizer, they will be resized to fit into the sizer.  When there is not
enough space in the sizer, the default behavior is to behave as a vertical box
sizer i.e. expanding the sizer in the vertical direction.
