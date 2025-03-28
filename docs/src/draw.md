@page draw Drawing

[TOC]

This chapter discusses how drawing works and how rectangles are marked for
redraw.

@section draw_loop Damaging, Drawing, and the Widget Tree

Each widget has an egt::v1::Widget::box(), which holds the bounding rectangle for
the widget. This is the rectangle a widget is responsible for at some z-order
for what content should be placed inside that bounding box.

Widgets are organized into a tree.  At the top of the tree there is a top level
widget that is a special type of widget called an egt::v1::Window.  An
egt::v1::Window is a frame widget, which is a composing widget.  This is just
another way of saying a frame can deal with zero or more children widgets inside
of its egt::v1::Widget::box().

Widgets are essentially rectangles that are drawn.  The widget defines the
content of what is drawn, but all that matters to the widget is its rectangle
of content.  That widget rectangle is directly drawn into a single global
back buffer for the screen.  This is an internal buffer in memory.
This back buffer is continuously updated in place by redrawing damaged
rectangles using the widgets that cover it.  When all of the rectangles have
been drawn in a single iteration of the egt::v1::EventLoop, only the damaged
region of that back buffer is then copied to a display buffer.  The
screen may actually have more than one buffer in order to handle double
or triple buffering.  So, when switching between these buffers, the screen has
to manage damaged regions for each of the buffers.

This means there are two flows up and down the widget hierarchy.  One is
propagation of the damaged rectangles, and the other is propagation of redrawing
the damaged rectangles with the correct content.

Damage propagates *up* in the tree.  If a leaf node widget is damaged, it will
ultimately report the damaged rectangle to a parent frame that has a surface.
This is usually the top level frame or the special plane frame.

@image html widget_hierarchy_damage.png "Damage"
@image latex widget_hierarchy_damage.png "Damage" width=8cm

Drawing goes the other way and is initiated by the EventLoop.  The
EventLoop will call on all top level frames and special plane frames to
redraw their damaged regions.

@image html widget_hierarchy_draw.png "Draw"
@image latex widget_hierarchy_draw.png "Draw" width=8cm

@section draw_paint Painting

Typically, drawing should only be done inside the egt::v1::Widget::draw()
method and its overrides in widget classes, which all descend from the
egt::v1::Widget class.

Sometimes, EGT default widgets have another level of indirection to implement
the draw() function by using the egt::v1::Drawer template.  This allows draw
functions to be overwritten by the theme.  However, this is not a technical
requirement if there is no need to theme the draw() method of a widget.
See @ref colors_themes "Widget Theme" for more information.

When a widget is asked to draw, it uses the reference to a egt::v1::Painter
instance passed as the first parameter to its draw() method to perform all
drawing. Also, the drawing should be limited to the egt::v1::Rect rectangle
passed as the second parameter to the draw() method. Indeed, this rectangle
represents one damaged region that should be redrawn during the
egt::v1::EventLoop.

@section draw_painter_model The egt::v1::Painter class's Drawing Model

Each egt::v1::Painter class instance is constructed from a egt::v1::Surface
class instance, which represents the **destination** or target where the
egt::v1::Painter instance draws anything.
If needed, this surface can be retrieved with the egt::v1::Painter::target()
method.

Then the **source** should be seen as the texture to be printed over the
**destination**. The **source** is generally set with the
egt::v1::Painter::source() method from either a:
- egt::v1::Color or egt::v1::Pattern : to apply a flat color or a gradient of colors.
- egt::v1::Surface or egt::v1::Image : to apply an image.

Finally the **mask** is a grayscale image that controls where the
egt::v1::Painter instance applies the **source** to the **destination**.
Indeed, any pixel in the **source** is blended into the **destination**
proportionally to the opacity of the corresponding pixel in the **mask**.
Hence, if the pixel is black in the **mask**, then the corresponding pixel in
the **source** is copied into the **destination**, whereas if the pixel is
white in the **mask**, then the **destination** is left unchanged.

@imageSize{architecture.png,width:500px;}
@image html cairo_drawing.png "Cairo Drawing Model [CC BY-SA 3.0 (https://creativecommons.org/licenses/by-sa/3.0)]"
@image latex cairo_drawing.png "Cairo Drawing Model [CC BY-SA 3.0 (https://creativecommons.org/licenses/by-sa/3.0)]" width=8cm

The **mask** may be set from a egt::v1::Surface or a egt::v1::Image with the
egt::v1::Painter::mask() method. However, most of the time, the **mask** is
implicitly built from a **path**. The **path** can be as basic as a simple
rectangle. Actually, the egt::v1::Painter class API and its implementation are
focused on drawing rectangles efficiently.

Nonetheless, the egt::v1::Painter API also provides other basic 2D primitive
functions to build a custom **path** made of lines, arcs or curves.
Moreover, when alpha blending is enabled with the
egt::v1::Painter::alpha_blending() method, the **mask** is also built from the
alpha channel of the **source**, if any.

When using the egt::v1::Painter class to draw, there are also several other
common properties and resources used. For example, the egt::v1::Palette of the
widget is usually referenced, along with its egt::v1::Font when applicable, and
also the use of egt::v1::Pattern and egt::v1::Color comes into play.

@section draw_function Implementing a Draw Function

The best way to see how typical widgets are drawn is to look at the draw()
functions of default EGT widgets.  However, there are several things to take
into account when implementing a draw() function.

By default, a parent frame will first clip the drawing area to the box() of the
widget that it is asked to draw().  This means that even if the widget tries to
draw outside of its box(), it will not be seen.  This is important because EGT
needs to know where things are drawn so it can account for repainting as
necessary. If any widget were allowed to draw anywhere on the screen at any time,
EGT would have no idea what needs to be redrawn.  To circumvent this operation
at your own risk, set the egt::v1::Widget::Flag::no_clip flag on a widget.  This will
disable automatic clipping.

When the parent frame calls the draw() function of a widget, it will first setup
an egt::v1::Painter::AutoSaveRestore instance.  Therefore, there is no need to handle saving or
restoring the state in any egt::v1::Widget::draw() function as this is done automatically.

@section draw_zorder Z-order

The z-order of widgets is the order in which they are drawn or composed.  It is
also the order in which they receive events through the widget hierarchy as
explained in @ref events_prop. Widgets are drawn from the bottom up.  By
default, widgets are drawn in the order in which they are added to a Frame.

If you add() a Label, and then add() a background ImageLabel that covers the
entire window, you will not see the label unless the background image is
transparent. To change this, you have to modify the z-order of the widgets by
either lowering the ImageLabel with egt::v1::Widget::zorder_down() or raising
the Label with egt::v1::Widget::zorder_up().
