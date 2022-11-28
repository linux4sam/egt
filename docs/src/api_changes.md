@page api_changes API Changes

[TOC]

Here, you can find the API changes from the version 1.0. In addition, some hints
are provided to help you update your code.

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
