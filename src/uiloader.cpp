/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "detail/base64.h"
#include "detail/egtlog.h"
#include <egt/themes/coconut.h>
#include <egt/themes/lapis.h>
#include <egt/themes/midnight.h>
#include <egt/themes/shamrock.h>
#include <egt/themes/sky.h>
#include <egt/themes/ultraviolet.h>
#include "egt/ui"
#include "egt/uiloader.h"
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>

namespace egt
{
inline namespace v1
{
namespace experimental
{

class EGT_API XmlDeserializer : public Deserializer
{
public:
    XmlDeserializer(rapidxml::xml_node<>* node)
        : m_node(node)
    {}

    XmlDeserializer() = default;
    XmlDeserializer(const XmlDeserializer&) = delete;
    XmlDeserializer& operator=(const XmlDeserializer&) = delete;
    XmlDeserializer(XmlDeserializer&&) noexcept = default;
    XmlDeserializer& operator=(XmlDeserializer&&) noexcept = default;
    ~XmlDeserializer() noexcept override = default;

    bool is_valid() const override;
    std::unique_ptr<Deserializer> first_child(const std::string& name = "") const override;
    std::unique_ptr<Deserializer> next_sibling(const std::string& name = "") const override;
    std::shared_ptr<Widget> parse_widget() const override;
    bool get_property(const std::string& name, std::string* value, Serializer::Attributes* attrs = nullptr) const override;

private:
    rapidxml::xml_node<>* m_node{nullptr};
};

bool XmlDeserializer::is_valid() const
{
    return m_node;
}

std::unique_ptr<Deserializer> XmlDeserializer::first_child(const std::string& name) const
{
    auto ret = std::make_unique<XmlDeserializer>();

    if (is_valid())
        ret->m_node = m_node->first_node(name.c_str());

    return ret;
}

std::unique_ptr<Deserializer> XmlDeserializer::next_sibling(const std::string& name) const
{
    auto ret = std::make_unique<XmlDeserializer>();

    if (is_valid())
        ret->m_node = m_node->next_sibling(name.c_str());

    return ret;
}

bool XmlDeserializer::get_property(const std::string& name, std::string* value, Serializer::Attributes* attrs) const
{
    for (const auto* prop = m_node->first_node("property");
         prop;
         prop = prop->next_sibling("property"))
    {
        const auto* attr = prop->first_attribute("name");
        if (attr && !strcmp(attr->value(), name.c_str()))
        {
            if (value)
                *value = prop->value();

            if (attrs)
            {
                attrs->clear();
                for (attr = prop->first_attribute(); attr; attr = attr->next_attribute())
                {
                    if (!strcmp(attr->name(), "name"))
                        continue;

                    attrs->emplace_back(attr->name(), attr->value());
                }
            }

            return true;
        }
    }

    return false;
}

static const std::pair<const std::string, std::function<std::unique_ptr<egt::Theme>()>> theme_items[] =
{
    {"CoconutTheme", []{ return std::make_unique<egt::CoconutTheme>(); }},
    {"LapisTheme", []{ return std::make_unique<egt::LapisTheme>(); }},
    {"MidnightTheme", []{ return std::make_unique<egt::MidnightTheme>(); }},
    {"SkyTheme", []{ return std::make_unique<egt::SkyTheme>(); }},
    {"ShamrockTheme", []{ return std::make_unique<egt::ShamrockTheme>(); }},
    {"UltraVioletTheme", []{ return std::make_unique<egt::UltraVioletTheme>(); }}
};

template <class T>
static std::shared_ptr<Widget> create_widget(rapidxml::xml_node<>* node,
        Serializer::Properties& saved_props)
{
    auto wname = node->first_attribute("name");

    Serializer::Properties props;
    if (node->first_node("property"))
    {
        for (auto prop = node->first_node("property"); prop; prop = prop->next_sibling("property"))
        {
            auto name = prop->first_attribute("name");
            if (!name)
            {
                detail::warn("property with no name");
                continue;
            }

            const std::string pname = name->value();
            std::string pvalue;
            Serializer::Attributes attrs;
            if (pname == "color")
            {
                auto pattern = prop->first_node("pattern");
                if (pattern)
                {
                    for (auto pprop = pattern->first_node("property"); pprop; pprop = pprop->next_sibling("property"))
                    {
                        auto ppname = pprop->first_attribute("name");
                        if (!ppname)
                        {
                            detail::warn("pattern with no name");
                            continue;
                        }
                        attrs.emplace_back(ppname->value(), pprop->value());
                    }
                }
                else
                {
                    pvalue = prop->value();
                }
            }
            else
            {
                pvalue = prop->value();
            }
            for (const rapidxml::xml_attribute<>* attr = prop->first_attribute(); attr;
                 attr = attr->next_attribute())
            {
                if (attr->name() == std::string("name"))
                    continue;

                attrs.emplace_back(attr->name(), attr->value());
            }

            props.emplace_back(std::make_tuple(pname, pvalue, attrs));
        }

        auto instance = std::make_shared<T>(props);
        if (wname)
            instance->name(wname->value());

        saved_props = std::move(props);

        return std::static_pointer_cast<Widget>(instance);
    }
    return nullptr;
}

using CreateFunction =
    std::function<std::shared_ptr<Widget>(rapidxml::xml_node<>* widget,
            Serializer::Properties& saved_props)>;

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
    {"egt::v1::FileOpenDialog", create_widget<FileOpenDialog>},
    {"egt::v1::FileSaveDialog", create_widget<FileSaveDialog>},
    {"egt::v1::FlexBoxSizer", create_widget<FlexBoxSizer>},
    {"egt::v1::Frame", create_widget<Frame>},
    {"egt::v1::HorizontalBoxSizer", create_widget<HorizontalBoxSizer>},
    {"egt::v1::ImageButton", create_widget<ImageButton>},
    {"egt::v1::ImageLabel", create_widget<ImageLabel>},
    {"egt::v1::Label", create_widget<Label>},
    {"egt::v1::LevelMeter", create_widget<LevelMeter>},
    {"egt::v1::LevelMeterF", create_widget<LevelMeterF>},
    {"egt::v1::LineWidget", create_widget<LineWidget>},
    {"egt::v1::ListBox", create_widget<ListBox>},
    {"egt::v1::ListBoxMulti", create_widget<ListBoxMulti>},
    {"egt::v1::Notebook", create_widget<Notebook>},
    {"egt::v1::NotebookTab", create_widget<NotebookTab>},
    //{"egt::v1::PopupVirtualKeyboard", create_widget<PopupVirtualKeyboard>},
    {"egt::v1::Picture", create_widget<Picture>},
    {"egt::v1::ProgressBar", create_widget<ProgressBar>},
    {"egt::v1::ProgressBarF", create_widget<ProgressBarF>},
    {"egt::v1::RadioBox", create_widget<RadioBox>},
    {"egt::v1::RectangleWidget", create_widget<RectangleWidget>},
    {"egt::v1::ScrolledView", create_widget<ScrolledView>},
    {"egt::v1::Scrollwheel", create_widget<Scrollwheel>},
    {"egt::v1::SelectableGrid", create_widget<SelectableGrid>},
    {"egt::v1::SideBoard", create_widget<SideBoard>},
    {"egt::v1::Slider", create_widget<Slider>},
    {"egt::v1::SliderF", create_widget<SliderF>},
    {"egt::v1::SpinProgress", create_widget<SpinProgress>},
    {"egt::v1::SpinProgressF", create_widget<SpinProgressF>},
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
    {"egt::v1::PointChart", create_widget<PointChart>},
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

static std::shared_ptr<Widget> parse_widget(rapidxml::xml_node<>* node)
{
    std::shared_ptr<Widget> result;
    Serializer::Properties props;
    std::string ttype;

    auto type = node->first_attribute("type");
    if (type)
        ttype = type->value();
    else
    {
        detail::warn("widget with no type");
        return nullptr;
    }

    bool found = false;
    for (const auto& i : allocators)
    {
        if (i.first == ttype)
        {
            result = i.second(node, props);
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
                result = x.second(node, props);
                break;
            }
        }

        if (!found)
        {
            detail::error("widget type {} unsupported", ttype);
            return nullptr;
        }
    }

    XmlDeserializer deserializer(node);
    result->deserialize_children(deserializer);

    result->post_deserialize(props);

    if (!props.empty())
    {
        for (auto& p : props)
        {
            detail::warn("unhandled {} property : {}", result->type(), std::get<0>(p));
        }
    }

    return result;
}

std::shared_ptr<Widget> XmlDeserializer::parse_widget() const
{
    return egt::v1::experimental::parse_widget(m_node);
}

static void parse_resource(rapidxml::xml_node<>* node)
{
    std::string name;

    auto n = node->first_attribute("name");
    if (n)
        name = n->value();
    else
    {
        detail::warn("resource with no name");
        return;
    }

    std::string buffer(node->value(), node->value_size());
    detail::strip(buffer);
    buffer = detail::base64_decode(buffer.data(), buffer.size());

    ResourceManager::instance().add(name.c_str(),
                                    std::vector<unsigned char>(buffer.begin(), buffer.end()));
}

template<class T>
static std::shared_ptr<Widget> load_document(T& doc)
{
    auto root = doc.first_node("egt");
    if (!root)
        return nullptr;

    auto resources = root->first_node("resources");
    if (resources)
    {
        for (auto resource = resources->first_node("resource");
             resource; resource = resource->next_sibling("resource"))
        {
            parse_resource(resource);
        }
    }

    auto gtheme = root->first_node("theme");
    if (gtheme)
    {
        for (auto tprop = gtheme->first_node("property"); tprop; tprop = tprop->next_sibling("property"))
        {
            auto tname = tprop->first_attribute("name");
            if (!tname)
            {
                detail::warn("theme property with no name");
                continue;
            }

            for (auto& i : theme_items)
            {
                if (i.first == tprop->value())
                {
                    egt::global_theme(std::move(i.second()));
                    break;
                }
            }
        }
    }

    auto gpalette = root->first_node("palette");
    if (gpalette)
    {
        egt::Palette palette;
        for (auto pprop = gpalette->first_node("property"); pprop; pprop = pprop->next_sibling("property"))
        {
            auto tname = pprop->first_attribute("name");
            if (!tname)
            {
                detail::warn("palette property with no name");
                continue;
            }

            std::string pname = tname->value();
            std::string pvalue = pprop->value();
            Serializer::Attributes pattrs;
            if (pname == "color")
            {
                for (const rapidxml::xml_attribute<>* attr = pprop->first_attribute(); attr;
                     attr = attr->next_attribute())
                {
                    pattrs.emplace_back(attr->name(), attr->value());
                }

                if (pvalue.empty())
                {
                    auto pattern = pprop->first_node("pattern");
                    if (pattern)
                    {
                        for (auto patprop = pattern->first_node("property"); patprop; patprop = patprop->next_sibling("property"))
                        {
                            auto patname = patprop->first_attribute("name");
                            if (!patname)
                            {
                                detail::warn("pattern property with no name");
                                continue;
                            }
                            pattrs.emplace_back(patname->value(), patprop->value());
                        }
                    }
                }
                palette.deserialize(pname, pvalue, pattrs);
            }
            else
            {
                detail::error("invalid palette property : {} = {}", pname, pprop->value());
            }

            if (!palette.empty())
                egt::global_palette(std::make_unique<egt::Palette>(palette));
        }
    }

    auto gfont = root->first_node("font");
    if (gfont)
    {
        egt::Font dfont;
        for (auto fprop = gfont->first_node("property"); fprop; fprop = fprop->next_sibling("property"))
        {
            auto fname = fprop->first_attribute("name");
            if (!fname)
            {
                detail::warn("font property with no name");
                continue;
            }

            std::string fontname = fname->value();
            std::string fontvalue;
            Serializer::Attributes fontattrs;
            if (fontname == "font")
            {
                fontvalue = fprop->value();

                for (const rapidxml::xml_attribute<>* attr = fprop->first_attribute(); attr;
                     attr = attr->next_attribute())
                {
                    fontattrs.emplace_back(attr->name(), attr->value());
                }

                dfont.deserialize(fontname, fontvalue, fontattrs);
                egt::global_font(std::make_unique<egt::Font>(dfont));
            }
        }
    }

    auto widgets = root->first_node("widgets");
    if (widgets)
    {
        for (auto widget = widgets->first_node("widget");
             widget; widget = widget->next_sibling("widget"))
        {
            // TODO: multiple root widgets not supported
            return parse_widget(widget);
        }
    }

    return nullptr;
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
        return load_document(doc);
    }
#ifdef EGT_HAS_HTTP
    case detail::SchemeType::network:
    {
        auto buffer = experimental::load_file_from_network<std::vector<char>>(path);
        if (!buffer.empty())
        {
            rapidxml::xml_document<> doc;
            doc.parse < rapidxml::parse_declaration_node | rapidxml::parse_no_data_nodes > (buffer.data());
            return load_document(doc);
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
