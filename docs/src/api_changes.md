@page api_changes API Changes

[TOC]

Here, you can find the API changes from the version 1.0. In addition, some hints
are provided to help you update your code.

@section v1_11 1.11

@subsection v1_11_application Application

@li The egt::v1::Application::gpu_enabled() getter tells whether the GPU, if any, is enabled. Also, the egt::v1::Application::enable_gpu() setter enables or disables at runtime the GPU. Finally, the EGT_GPU_DISABLED environment variable disables the GPU at the application startup: if not defined, the GPU, if any, is enabled by default.

@subsection v1_11_arctype ArcType

@li A new constructor now allows implicit conversions between egt::v1::ArcType template instances with different 'Dim' template parameters; for instance between ArcType<DefaultDim> and ArcType<float>.

@subsection v1_11_canvas Canvas

@li The egt::v1::Canvas class has been removed and should be replaced with a egt::v1::Surface and/or a egt::v1::Painter created from this egt::v1::Surface.
@code{.unparsed}
- egt::Canvas canvas(...);
- egt::Painter painter(canvas.context());
+ egt::Surface target(...);
+ egt::Painter painter(target);
@endcode

@subsection v1_11_circletype CircleType

@li A new constructor now allows implicit conversions between egt::v1::CircleType template instances with different 'Dim' template parameters; for instance between CircleType<DefaultDim> and CircleType<float>.

@subsection v1_11_ellipsetype EllipseType

@li A new constructor now allows implicit conversions between egt::v1::EllipseType template instances with different 'Dim' template parameters; for instance between EllipseType<DefaultDim> and EllipseType<float>.

@subsection v1_11_font Font

@li The new egt::v1::Font::text_size() method is a helper method: it internally calls egt::v1::Painter::text_size() on a default Painter instance, which has no transformation like rotation or symmetry. If you want to transform the font, then you should call egt::v1::Painter::text_size() instead, on the relevant Painter instance.

@li The egt::v1::Font::scaled_font() method now returns a 'const detail::InternalFont&' rather than a 'cairo_scaled_font_t*' previously: detail::InternalFont is an opaque class to hide the internal implementation.

@li The new egt::v1::Font::extents() methods return either a egt::v1::Font::FontExtents or egt::v1::Font::TextExtents, which abstract the 'cairo_font_extents_t' and 'cairo_text_extents_t' types. These new extents() methods internally call the new egt::v1::Painter::extents() methods on a default Painter instance, which has no transformation like rotation or symmetry. If you want to transform the font, then you should call egt::v1::Painter::extents() instead, on the relevant Painter instance.

@subsection v1_11_image Image

@li The egt::v1::Image::Image(shared_cairo_surface_t) constructor has been replaced with the new egt::v1::Image::Image(std::shared_ptr<Surface>) constructor.

@li The egt::v1::Image::Image(cairo_surface_t*) constructor has been removed in the process of abstracting cairo.

@li New egt::v1::Image::Image(Surface&&) and egt::v1::Image::Image(Surface&&, const std::string&) constructors have been added and called from 'egt::v1::SvgImage::operator Image()' and egt::v1::SvgImage::render().

@li The egt::v1::Image::surface() method now returns a 'const std::shared_ptr<Surface>&' reference rather than a copy of a 'shared_cairo_surface_t'.

@li The egt::v1::Image::pattern(), egt::v1::Image::crop() and egt::v1::Image::copy() methods have been removed.

@subsection v1_11_painter Painter

@li The new egt::v1::Painter::Painter(Surface&) constructor replaces the former egt::v1::Painter::Painter(shared_cairo_t) constructor.

@li New egt::v1::Painter::target() getters have been added to retrieve the egt::v1::Surface used to construct a egt::v1::Painter.

@li The egt::v1::Painter::context() method now returns a 'const detail::InternalContext&' rather than a 'shared_cairo_t' previously: detail::InternalContext is an opaque class to hide the internal implementation.

@li New methods were added in the process of abstracting cairo and must called from EGT applications as replacement of the egt::v1::Painter::context() method followed by functions of the cairo API.
@code{.unparsed}
  egt::Painter painter = ...;
  egt::Image image = ...;
  float angle = ...;
  egt::Point center = ...;
  egt::Point image_orig = ...;
- auto cr = painter.context().get();

- cairo_save(cr);
- cairo_translate(cr, center.x(), center.y());
- cairo_rotate(cr, angle);
- cairo_set_source_surface(cr, image.surface().get(), image_orig.x(), image_orig.y());
- cairo_paint(cr);
- cairo_restore(cr);
+ Painter::AutoSaveRestore sr(painter);
+
+ painter.translate(center);
+ painter.rotate(angle);
+ painter.source(image, image_orig);
+ painter.paint();
@endcode
    * egt::v1::Painter::alpha_blending() getter and setter to abstract the cairo_set_operator() function with either CAIRO_OPERATOR_OVER or CAIRO_OPERATOR_SOURCE.
    * egt::v1::Painter::antialias() getter and setter to abstract the cairo_get_antialias() and cairo_set_antialias() functions.
    * egt::v1::Painter::arc() method to abstract the cairo_arc() function.
    * egt::v1::Painter::extents() methods to abstract the cairo_font_extents() and cairo_text_extents() functions.
    * egt::v1::Painter::fill_preserve() method to abstract the cairo_fill_preserve() function.
    * egt::v1::Painter::line_cap() getter and setter to abstract the cairo_get_line_cap() and cairo_set_line_cap() functions.
    * egt::v1::Painter::line_to() method to abstract the cairo_line_to() function.
    * egt::v1::Painter::move_to() method to abstract the cairo_line_to() function.
    * egt::v1::Painter::rectangle() method to abstract the cairo_rectangle() function.
    * egt::v1::Painter::rotate() method to abstract the cairo_rotate() function.
    * egt::v1::Painter::scale() method to abstract the cairo_scale() function.
    * egt::v1::Painter::set_dash() method to abstract the cairo_set_dash() function.
    * egt::v1::Painter::show_text() methods to abstract the cairo_show_text() functions.
    * egt::v1::Painter::source() methods to abstract the cairo_set_source(), cairo_set_source_rgba() and cairo_set_source_surface() functions.
    * egt::v1::Painter::translate() method to abstract the cairo_translate() function.

@li Some egt::v1::Painter::draw() overloads have been created or updated to draw egt::v1::Image, egt::v1::Surface and egt::v1::Pattern instances more efficiently.

@li New egt::v1::Painter::low_fidelity() and egt::v1::Painter::high_fidelity() methods duplicate what the egt::v1::Screen::{low,high}_fidelity() methods do making the feature available for any egt::v1::Painter instance.

@subsection v1_11_pattern Pattern

@li The egt::v1::Pattern::pattern() method now returns a 'const detail::InternalPattern&' rather than a 'cairo_pattern_t*' previously: detail::InternalPattern is an opaque class to hide the internal implementation.

@subsection v1_11_resourcemanager ResourceManager

@li The egt::v1::detail::read_resource_stream() function has been removed.

@subsection v1_11_screen Screen

@li The 'shared_cairo_t egt::v1::Screen::context()' method has been removed and replaced with the new 'Painter& egt::v1::Screen::painter()' method in the process of abstracting cairo.

@subsection v1_11_slider Slider

@li The new egt::v1::SliderBase::SliderFlag::hide_line flag, when set, allows to skip drawing the slider line.

@li The new egt::v1::Slider::label_offset() setter and getter control how much the text label is shifted from the handle position, perpendicularly to the slider line where the handle moves on.

@li The new egt::v1::Slider::handle_margin() setter and getter control the stroke of the handle; when negative, the handle can cross the widget box (but still without being drawn outside this box).

@li The new egt::v1::Slider::handle_offset() setter and getter control how much the handle is shifted from the middle of the slider box.

@subsection v1_11_sprite Sprite

@li The egt::v1::Sprite::surface() method has been removed.

@subsection v1_11_surface Surface

@li The new egt::v1::Surface class has been introduced to abstract the 'cairo_surface_t' type.

@subsection v1_11_widget Widget

@li The egt::v1::Widget::paint() method is no longer virtual, hence all overrides have been removed.

@li The egt::v1::Widget::paint_to_file() method is no longer virtual, hence all overrides have been removed.

@section v1_10 1.10

@subsection cameraWindow CameraWindow

@li The start() method should be replaced with the play() method. To standardize across multimedia components, the start() method only creates the GStreamer pipeline. To transition its state to play, the play() method must be utilized. The play() method will automatically invoke the start() method if the pipeline has not been previously created.

@section v1_9 1.9

@subsection checkbutton CheckButton

@li The class is added to the API.

@li The handle() method is now public as it should have been.

@subsection event Event

@li Add postpone_stop() and posptponed_quit() methods to handle event propagation.

@subsection imagegroup ImageGroup

@li New utility class to handle images tied to a palette group id.

@subsection inputlibinput InputLibInput

@li Add an optional device argument to the constructor.

@subsection listbox ListBox

@li Add orient() getter and setter to handle the orientation: either vertical or horizontal.

@li Add scroll_beginning/end() methods.

@subsection mousegesture MouseGesture

@li Add drag_enable_distance() setter and getter to configure the distance to travel to enable drag mode.

@subsection painter Painter

@li Add translate() and rotate() methods to abstract Cairo translation and rotation.

@subsection pointtype PointType

@li Add unary 'operator-'.

@subsection rangevalue RangeValue

@li Add rbegin(), rend(), crbegin(), and crend() iterators.

@subsection scrolledview ScrolledView

@li Add init_sliders() method.

@li Rename resize_slider() into resize_sliders();

@subsection slider Slider

@li Add format_label() method to handle the formatting of the value.

@li Add handle_image() setter and getter, and the reset_handle_method() to manage an optional image to draw for the handle.

@li Fix the prototype of the to_offset() method. The 'value' parameter should be 'T' rather than 'int'.

@subsection switch Switch

@li Add the Switch class to factorize code from CheckBox and RadioBox which now inherits from it.

@subsection valuerangewidget ValueRangeWidget

@li Add an optional step parameter to the constructor and stepping() getter and setter. Step describes the difference between two consecutive values of the range.

@li Add the set_value() method to factorize code.

@subsection widget Widget

@li children() returns a reference rather than a value.

@li Add init() method to factorize code.

@li Add components() getters.

@li Add update_subordinates_ranges() method to factorize code.

@li Add the range_from_widget() method to return the proper subordinate range, either children or components depending on the type of the subordinate given as argument.

@li Constify local_to_display() and display_to_local() methods.

@li Add the point_from_subordinate() virtual method to return the widget point coordinates, but from the point of view of one of its subordinates.

@li Add the hit() method to test whether a DisplayPoint lays within a widget box.

@li Add dragged() setter and getter for the current widget which is being dragged.

@li Add group() method to retrieve the palette group id depending on the widget state.

@li Add background() setter and getter, and the reset_background() method to handle the image background per palette group.

@li The has_screen() method is now public.

@section v1_8 1.8

@subsection progressbar ProgressBar

@li Add style property to define the direction of the progress bar: left to right, right to left, bottom to top, or top to bottom.

@subsection sound Sound

@li Add enumarate_pcm_device() method to get a list of device names available.

@li Add media() method to set another file to play.

@li Add stop() method.

@section v1_7 1.7

@subsection detail Detail

@li add demangle() function to the API.

@subsection filesystem Filesystem (C++ stdlib)

@li move to c++17 std::filesystem
@code{.unparsed}
- #include <experimental/filesystem>
- namespace fs = std::experimental::filesystem;
+ #include <filesystem>
+ namespace fs = std::filesystem;
@endcode

@subsection painter Painter

@li remove the font_size() method as it mostly duplicates the text_size() method.

@subsection serializer Serializer

@li Remove add_node() and previous_node() methods.

@li Add begin_child() and end_child() methods.

@li Remove the level parameter of the add() method.

@subsection textbox TextBox

@li Remove justify parameter from prepare_text().

@li Remove justify and orient parameters from compute_layout().

@li Transform several static functions to instance methods: context(),
tokenize(), compute_layout(), consolidate(), clear_selection(), set_selection(),
get_line(), longest_prefix(), longest_suffix(), tag_default_aligned_line(),
tag_left_aligned_line(), tag_right_aligned_line(), tag_line(), tag_text(),
tag_line_selection(), tag_text_selection(), prepare_text(), get_cursor_rect(),
draw_text().

@li Add horizontal_scrollable and vertical_scrollable flags to TextFlag and some
methods to handle it: text_area(), init_sliders(), resize_sliders(),
update_hslider(), update_vslider(), update_sliders(), draw_sliders(),
damage_component(), damage_hslider(), damage_vslider(), move_hslider(),
move_vslider(), move_sliders().

@li Add point2pos() method.

@li Add beginning_of_line() and end_of_line() methods.

@li Add up() and down() methods to handle UP and DOWN keys.

@subsection widget Widget

@li Children handling has been moved from Frame to Widget. It should be
transparent for users. The main change is the set_parent() method that takes a
Widget* parameter instead of a Frame* parameter. Now, Widget can be used to
create widgets composed of several components. Prefer inheriting from Widget if
users must not be able to use add or remove.

@section v1_6 1.6

@subsection cxx C++

@li Switch to C++17

@subsection frame Frame

@li Add zorder setter to change the rank of a child.

@subsection pattern Pattern

@li Add operator== and operator!=.

@subsection widget Widget

@li Add zorder setter to change the rank.

@subsection widgetflags Widgetflags

@li Transform the WidgetFlag enum into a WidgetFlag class plus few other
related classes: HorizontalBitField, VerticalBitField, ExpandBitField,
HVBitField, HExpandBitField, VExpendBitField.

@li Override operators| to allow only valid combinations of WidgetFlag and
related classes listed above.

@section v1_5 1.5

@subsection widget Widget

@li Add a setter for the focus.

@subsection textbox TextBox

@li Make the handle_key() method virtual.

@section v1_4 1.4

@subsection app App

@li quit() method accepts an optional parameter corresponding to the exit value.

@subsection chartbase ChartBase

@li Add bank setter and getter.

@li Add xlabel, ylbabel and title setters.

@li Introduce ChartItemArray to replace DataArray and StringDataArray which were
specific to each ChartBase subclass.
@code{.unparsed}
- PieChart::StringDataArray data;
- data.push_back(make_pair(25, "truck"));
+ egt::ChartItemArray data;
+ data.add(25, "truck");
@endcode

@subsection checkbox CheckBox

@li Add show_label setter and getter.

@li Add checkbox_align setter and getter to set the alignement of the checkbox
relative to the text.

@subsection combobox ComboBox

@li Add another add_item() method that do not delegate the ownership of the
StringItem to the ComboBox.

@subsection font Font

@li Add global_font setter and getter, and reset_global_font() method to
manage a global font. So if a theme is used only to handle a palette and a
font, prefer using the global font.

@subsection grid Grid

@li Add gride_size setter and getter.

@subsection image Image

@li Add uri() and reset_uri() methods to set/unset the image URI.

@li load() method takes an additional optional parameter to indicate if the
scale can be approximated to increase iamge cache hit efficiency.

@subsection imageholder ImageHolder

@li Add uri setter and getter.

@li Add reset_uri() method.

@li Add hscale, vscale and scale setters and getters.

@subsection listbox ListBox

@li Add another add_item() method that do not delegate the ownership of the
StringItem to the ListBox.

@subsection palette Palette

@li Add global_palette setter and getter, and reset_global_palette() method to
manage a global palette. So if a theme is used only to handle a palette and a
font, prefer using the global palette.

@li Add empty() method to check if a palette is empty or not.

@subsection radiobox RadioBox

@li Add show_label setter and getter.

@li Add radiobox_align setter and getter to set the alignement of the radiobox
relative to the text.

@subsection scrolledview ScrolledView

@li Add hoffset and voffset getters.

@li Add hpolicy and vpolicy getters and setters.

@subsection slidertype SliderType

@li Add live_update getter.

@subsection textbox TextBox

@li Add text_flags setter.

@subsection theme Theme

@li Add name setter and getter as themes can be identified with their name.

@li Remove constructor dedicated to de-serialization and the serialization
method. As a theme is more than a palette and a font, it can draw in a different
way widgets, it can't be serialized entirely. So remove this feature.

@subsection widget Widget

@li Add has_font() method to check if the widget has a custom font.

@li Add reset_font() method to unset the widget's custom font.

@li Add has_palette() method to check if the widget has a custom palette.

@li Add palette getter to retrieve which palette is used by the widget.

@li Add reset_palette() method to unset the widget's custom font.

@li Remove theme getter and reset_theme() method. Local theme are no longer
supported at the widget level. There is only one theme which is the global
theme. If a local theme was used to set a palette and/or a font, prefer using
the local palette/font. If it was used to draw the widget in a different way,
prefer having a subclass of this widget where you override the draw method.

@li deseriliaze() method is no longer virtual as it's only called from
constructors.

@subsection window Window

@li Add format_hint() setter and getter.

@li Add window_hint() setter and getter.

@section v1_3 1.3

@subsection animationsequence AnimationSequence

@li Add removeLast() method to remove the last sub animation from the sequence.

@li Add removeAt() method to remove the sub animation at the given position from
the sequence.

@subsection combobox ComboBox

@li Add default_size() setter and getter.

@subsection image Image

@li Add keep_image_ratio() setter and getter.

@subsection slider SliderType

@li Add min_size_hint() getter.

@subsection textbox TextBox

@li Add selection_forward() and selection_backward() method to extend the
selection.

@section v1_2 1.2

@subsection rangevalue RangeValue

@li Rename min and max to start and end. The constraint to get max > min is
removed.

@subsection slider Slider

@li The constraint to get start > end is removed.
@li The origin_opposite flag is removed. As end can be greater than start, swap
end and start instead of setting the origin_opposite flag.

@subsection valuerangewidget ValueRangeWidget

@li The constraint to get start > end is removed.

@subsection imagebutton ImageButton

@li Add keep_image_ratio() setter and getter. The keep image ratio state is set
to true by default. When enabled, the image ratio is kept during the image auto
scale operation.

@subsection imagelabel ImageLabel

@li Add keep_image_ratio() setter and getter. The keep image ratio state is set
to true by default. When enabled, the image ratio is kept during the image auto
scale operation.

@section v1_1 1.1

@subsection combobox ComboBox

The changes about ComboBox are about providing a common interface with ListBox.

@li Rename remove() to remove_item().
@li Uses StringItem as item type instead of string.
@code{.unparsed}
- combo->add_item("Item1");
+ combo->add_item(std::make_shared<egt::StringItem>("Item1"))
@endcode
@code{.unparsed}
auto s = combo->item_at(i);
- if (s == "ok")
+ if (s->text() == "ok")
@endcode
@li Add show() and hide() member functions.

@subsection listbox ListBox

@li Restrict items to StringItem instead of Widgets.

@subsection staticgrid StaticGrid

@li Add n_col() and n_row() member functions to get the number of columns and
rows.
@li Add add() member functions taking a GridPoint as parameter instead of column
and row. Prefer these new member functions to the older ones.
@code{.unparsed}
- grid.add(button, 2, 1);
+ grid.add(button, egt::StaticGrid::GridPoint(2, 1));
@endcode
@li Layout changes: the border of the widget is no longer used to space out
cells. Remove the border parameter from the constructor and add
horizontal/vertical_space getters and setters.
@code{.unparsed}
- auto grid = make_shared<StaticGrid>(StaticGrid::GridSize(2, 2), 1);
+ auto grid = make_shared<StaticGrid>(StaticGrid::GridSize(2, 2));
+ instance->horizontal_space(1);
+ instance->vertical_space(1);
@endcode

@subsection imagebutton_label ImageButton/Label

@li Add auto_scale_image() setter and getter. The auto scale image state is set
to true by default. When enabled, the image will be automatically scaled down to
fit the box attributed by the align algorithm.
