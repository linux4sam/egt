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
static std::shared_ptr<Widget> create_widget(rapidxml::xml_node<>* node,
        const std::shared_ptr<Frame>& parent)
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

    for (auto prop = node->first_node("property"); prop; prop = prop->next_sibling("property"))
    {
        auto name = prop->first_attribute("name");
        if (!name)
        {
            spdlog::warn("property with no name {}", prop->value());
            continue;
        }

        std::string pname = name->value();
        const std::string pvalue = prop->value();

        Serializer::Attributes attrs;
        for (const rapidxml::xml_attribute<>* attr = prop->first_attribute(); attr;
             attr = attr->next_attribute())
        {
            if (attr->name() == std::string("name"))
                continue;

            attrs.emplace_back(attr->name(), attr->value());
        }

        instance->deserialize(pname, pvalue, attrs);
    }

    return std::static_pointer_cast<Widget>(instance);
}

using CreateFunction =
    std::function<std::shared_ptr<Widget>(rapidxml::xml_node<>* widget,
            const std::shared_ptr<Frame>& parent)>;

static const std::pair<std::string, CreateFunction> allocators[] =
{
    {"egt::v1::experimental::Radial", create_widget<Radial>},
    {"egt::v1::AnalogMeter", create_widget<AnalogMeter>},
    {"egt::v1::BoxSizer", create_widget<BoxSizer>},
    {"egt::v1::Button", create_widget<Button>},
    //{"egt::v1::ButtonGroup", create_widget<ButtonGroup>},
    {"egt::v1::CheckBox", create_widget<CheckBox>},
    {"egt::v1::CheckButton", create_widget<CheckButton>},
    {"egt::v1::CircleWidget", create_widget<CircleWidget>},
    {"egt::v1::ComboBox", create_widget<ComboBox>},
    {"egt::v1::Dialog", create_widget<Dialog>},
    //{"egt::v1::FileDialog", create_widget<FileDialog>},
#ifdef EGT_HAS_FILEDIALOG
    {"egt::v1::FileOpenDialog", create_widget<FileOpenDialog>},
    {"egt::v1::FileSaveDialog", create_widget<FileSaveDialog>},
#endif
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
#ifdef EGT_HAS_VIRTUALKEYBOARD
    {"egt::v1::VirtualKeyboard", create_widget<VirtualKeyboard>},
#endif
    {"egt::v1::Window", create_widget<Window>},
    {"egt::v1::Form", create_widget<Form>},
    //{"egt::v1::Gauge", create_widget<Gauge>},
    //{"egt::v1::GaugeLayer", create_widget<GaugeLayer>},
    //{"egt::v1::NeedleLayer", create_widget<NeedleLayer>},
#ifdef EGT_HAS_CHART
    {"egt::v1::LineChart", create_widget<LineChart>},
    {"egt::v1::PointChart", create_widget<PieChart>},
    {"egt::v1::BarChart", create_widget<BarChart>},
    {"egt::v1::HorizontalBarChart", create_widget<HorizontalBarChart>},
    {"egt::v1::PieChart", create_widget<PieChart>},
#endif
#ifdef EGT_HAS_CAMERA
    {"egt::v1::CameraWindow", create_widget<CameraWindow>},
#endif
#ifdef EGT_HAS_VIDEO
    {"egt::v1::VideoWindow", create_widget<VideoWindow>},
#endif
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

    bool found = false;
    for (const auto& i : allocators)
    {
        if (i.first == ttype)
        {
            result = i.second(node, parent);
            found = true;
        }
    }

    if (!found)
    {
        for (const auto& x : allocators)
        {
            auto name = x.first;
            const std::size_t index = name.find_last_of(':');
            if (index != std::string::npos)
                name = name.substr(index + 1);

            if (ttype == name)
            {
                found = true;
                result = x.second(node, parent);
                break;
            }
        }

        if (!found)
        {
            spdlog::error("widget type {} unsupported", ttype);
            return nullptr;
        }
    }

    for (auto child = node->first_node("widget"); child; child = child->next_sibling("widget"))
    {
        parse_widget(child, std::dynamic_pointer_cast<Frame>(result));
    }

    return result;
}

std::shared_ptr<Widget> UiLoader::load(const std::string& uri)
{
    std::string path;
    auto type = detail::resolve_path(uri, path);

    switch (type)
    {
    case detail::SchemeType::filesystem:
    {
        rapidxml::file<> xml_file(path.c_str());
        rapidxml::xml_document<> doc;
        doc.parse < rapidxml::parse_declaration_node | rapidxml::parse_no_data_nodes > (xml_file.data());

        auto widgets = doc.first_node("widgets");
        for (auto widget = widgets->first_node("widget");
             widget; widget = widget->next_sibling("widget"))
        {
            // TODO: multiple root widgets not supported
            return parse_widget(widget);
        }

        break;
    }
#ifdef EGT_HAS_HTTP
    case detail::SchemeType::network:
    {
        auto buffer = experimental::load_file_from_network<std::vector<char>>(path);
        if (!buffer.empty())
        {
            rapidxml::xml_document<> doc;
            doc.parse < rapidxml::parse_declaration_node | rapidxml::parse_no_data_nodes > (buffer.data());

            auto widgets = doc.first_node("widgets");
            for (auto widget = widgets->first_node("widget");
                 widget; widget = widget->next_sibling("widget"))
            {
                // TODO: multiple root widgets not supported
                return parse_widget(widget);
            }
        }

        break;
    }
#endif
    default:
    {
        throw std::runtime_error("unsupported uri: " + uri);
    }
    }

    throw std::runtime_error("failed to parse any widget");

    return {};
}

}
}
}
