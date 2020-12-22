@page api_changes API Changes

[TOC]

Here, you can find the API changes from the version 1.0. In addition, some hints
are provided to help you update your code.

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
