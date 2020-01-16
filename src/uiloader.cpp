/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/ui"
#include "egt/uiloader.h"
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace egt
{
inline namespace v1
{
namespace experimental
{

template <class T>
static std::shared_ptr<Widget> create_widget(rapidxml::xml_node<>* node, const std::shared_ptr<Frame>& parent)
{
    auto instance = std::make_shared<T>();
    if (parent)
    {
        parent->add(instance);
    }

    auto name = node->first_attribute("name");
    if (name)
    {
        instance->name(name->value());
    }

    for (auto prop = node->first_node("property"); prop; prop = prop->next_sibling())
    {
        std::map<std::string, std::string> attrs;
        std::string pname;

        for (rapidxml::xml_attribute<>* attr = prop->first_attribute(); attr;
             attr = attr->next_attribute())
        {
            if (attr->name() == std::string("name"))
                pname = attr->value();
            else
                attrs.insert({attr->name(), attr->value()});
        }

        std::string pvalue = prop->value();

        instance->deserialize(pname, pvalue, attrs);
    }

    return std::static_pointer_cast<Widget>(instance);
}

using create_function =
    std::function<std::shared_ptr<Widget>(rapidxml::xml_node<>* widget,
            std::shared_ptr<Frame> parent)>;

static const std::map<std::string, create_function> allocators =
{
    {"AnalogMeter", create_widget<AnalogMeter>},
    {"BoxSizer", create_widget<BoxSizer>},
    {"Button", create_widget<Button>},
    //{"ButtonGroup", create_widget<ButtonGroup>},
    {"CameraWindow", create_widget<CameraWindow>},
    {"CheckBox", create_widget<CheckBox>},
    {"CheckButton", create_widget<CheckButton>},
    {"CircleWidget", create_widget<CircleWidget>},
    {"ComboBox", create_widget<ComboBox>},
    {"Dialog", create_widget<Dialog>},
    //{"FileDialog", create_widget<FileDialog>},
    {"FileOpenDialog", create_widget<FileOpenDialog>},
    {"FileSaveDialog", create_widget<FileSaveDialog>},
    {"FlexBoxSizer", create_widget<FlexBoxSizer>},
    {"Frame", create_widget<Frame>},
    {"HorizontalBoxSizer", create_widget<HorizontalBoxSizer>},
    {"ImageButton", create_widget<ImageButton>},
    {"ImageLabel", create_widget<ImageLabel>},
    {"Label", create_widget<Label>},
    {"LevelMeter", create_widget<LevelMeter>},
    {"LineWidget", create_widget<LineWidget>},
    {"ListBox", create_widget<ListBox>},
    {"Notebook", create_widget<Notebook>},
    {"NotebookTab", create_widget<NotebookTab>},
    //{"PopupVirtualKeyboard", create_widget<PopupVirtualKeyboard>},
    {"ProgressBar", create_widget<ProgressBar>},
    {"RadioBox", create_widget<RadioBox>},
    {"RectangleWidget", create_widget<RectangleWidget>},
    {"ScrolledView", create_widget<ScrolledView>},
    {"Scrollwheel", create_widget<Scrollwheel>},
    {"SelectableGrid", create_widget<SelectableGrid>},
    {"SideBoard", create_widget<SideBoard>},
    {"Slider", create_widget<Slider>},
    {"SpinProgress", create_widget<SpinProgress>},
    {"Sprite", create_widget<Sprite>},
    {"StaticGrid", create_widget<StaticGrid>},
    {"TextBox", create_widget<TextBox>},
    {"ToggleBox", create_widget<ToggleBox>},
    {"TopWindow", create_widget<TopWindow>},
    {"VerticalBoxSizer", create_widget<VerticalBoxSizer>},
    {"VideoWindow", create_widget<VideoWindow>},
    {"VirtualKeyboard", create_widget<VirtualKeyboard>},
    {"Window", create_widget<Window>},
    {"Form", create_widget<Form>},
    //{"Gauge", create_widget<Gauge>},
    //{"GaugeLayer", create_widget<GaugeLayer>},
    //{"NeedleLayer", create_widget<NeedleLayer>},
    //{"Radial", create_widget<Radial>},
    {"LineChart", create_widget<LineChart>},
    {"PieChart", create_widget<PieChart>},
    //{"BarChart", create_widget<BarChart>},
    //{"HBarChart", create_widget<HBarChart>},
    //{"PointChart", create_widget<PointChart>},


    {"RadialType<int>", create_widget<RadialType<int>>},
    {"egt::v1::experimental::RadialType<int>", create_widget<RadialType<int>>},

    {"egt::v1::AnalogMeter", create_widget<AnalogMeter>},
    {"egt::v1::BoxSizer", create_widget<BoxSizer>},
    {"egt::v1::Button", create_widget<Button>},
    //{"egt::v1::ButtonGroup", create_widget<ButtonGroup>},
    {"egt::v1::CameraWindow", create_widget<CameraWindow>},
    {"egt::v1::CheckBox", create_widget<CheckBox>},
    {"egt::v1::CheckButton", create_widget<CheckButton>},
    {"egt::v1::CircleWidget", create_widget<CircleWidget>},
    {"egt::v1::ComboBox", create_widget<ComboBox>},
    {"egt::v1::Dialog", create_widget<Dialog>},
    //{"egt::v1::FileDialog", create_widget<FileDialog>},
    {"egt::v1::FileOpenDialog", create_widget<FileOpenDialog>},
    {"egt::v1::FileSaveDialog", create_widget<FileSaveDialog>},
    {"egt::v1::FlexBoxSizer", create_widget<FlexBoxSizer>},
    {"egt::v1::Frame", create_widget<Frame>},
    {"egt::v1::HorizontalBoxSizer", create_widget<HorizontalBoxSizer>},
    {"egt::v1::ImageButton", create_widget<ImageButton>},
    {"egt::v1::ImageLabel", create_widget<ImageLabel>},
    {"egt::v1::Label", create_widget<Label>},
    {"egt::v1::LevelMeter", create_widget<LevelMeter>},
    {"egt::v1::LineWidget", create_widget<LineWidget>},
    {"egt::v1::ListBox", create_widget<ListBox>},
    {"egt::v1::Notebook", create_widget<Notebook>},
    {"egt::v1::NotebookTab", create_widget<NotebookTab>},
    //{"egt::v1::PopupVirtualKeyboard", create_widget<PopupVirtualKeyboard>},
    {"egt::v1::ProgressBar", create_widget<ProgressBar>},
    {"egt::v1::RadioBox", create_widget<RadioBox>},
    {"egt::v1::RectangleWidget", create_widget<RectangleWidget>},
    {"egt::v1::ScrolledView", create_widget<ScrolledView>},
    {"egt::v1::Scrollwheel", create_widget<Scrollwheel>},
    {"egt::v1::SelectableGrid", create_widget<SelectableGrid>},
    {"egt::v1::SideBoard", create_widget<SideBoard>},
    {"egt::v1::Slider", create_widget<Slider>},
    {"egt::v1::SpinProgress", create_widget<SpinProgress>},
    {"egt::v1::Sprite", create_widget<Sprite>},
    {"egt::v1::StaticGrid", create_widget<StaticGrid>},
    {"egt::v1::TextBox", create_widget<TextBox>},
    {"egt::v1::ToggleBox", create_widget<ToggleBox>},
    {"egt::v1::TopWindow", create_widget<TopWindow>},
    {"egt::v1::VerticalBoxSizer", create_widget<VerticalBoxSizer>},
    {"egt::v1::VideoWindow", create_widget<VideoWindow>},
    {"egt::v1::VirtualKeyboard", create_widget<VirtualKeyboard>},
    {"egt::v1::Window", create_widget<Window>},
    {"egt::v1::Form", create_widget<Form>},
    //{"egt::v1::Gauge", create_widget<Gauge>},
    //{"egt::v1::GaugeLayer", create_widget<GaugeLayer>},
    //{"egt::v1::NeedleLayer", create_widget<NeedleLayer>},
    //{"egt::v1::Radial", create_widget<Radial>},
    {"egt::v1::experimental::LineChart", create_widget<LineChart>},
    {"egt::v1::experimental::PieChart", create_widget<PieChart>},
    //{"egt::v1::BarChart", create_widget<BarChart>},
    //{"egt::v1::HBarChart", create_widget<HBarChart>},
    //{"egt::v1::PointChart", create_widget<PointChart>},
};

static std::shared_ptr<Widget> parse_widget(rapidxml::xml_node<>* node,
        const std::shared_ptr<Frame>& parent = nullptr)
{
    std::shared_ptr<Widget> result;
    std::string ttype;

    auto type = node->first_attribute("type");
    if (type)
    {
        ttype = type->value();
    }

    auto i = allocators.find(ttype);
    if (i != allocators.end())
    {
        result = i->second(node, parent);
    }
    else
    {
        spdlog::error("widget type {} unsupported", ttype);
        return nullptr;
    }

    for (auto child = node->first_node("widget"); child; child = child->next_sibling())
    {
        parse_widget(child, std::dynamic_pointer_cast<Frame>(result));
    }

    return result;
}

std::shared_ptr<Widget> UiLoader::load(const std::string& file)
{
    rapidxml::file<> xml_file(file.c_str());
    rapidxml::xml_document<> doc;
    doc.parse < rapidxml::parse_declaration_node | rapidxml::parse_no_data_nodes > (xml_file.data());

    auto widgets = doc.first_node("widgets");
    for (auto widget = widgets->first_node("widget");
         widget; widget = widget->next_sibling())
    {
        // TODO: multiple root widgets not supported
        return parse_widget(widget);
    }

    return std::shared_ptr<Widget>();
}

}
}
}
