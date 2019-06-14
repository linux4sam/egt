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
void set_widget_property(T instance, const std::string& name, const std::string& value)
{
    if (name == "width")
        instance->set_width(std::stoi(value));
    else if (name == "height")
        instance->set_height(std::stoi(value));
    else if (name == "x")
        instance->set_x(std::stoi(value));
    else if (name == "y")
        instance->set_y(std::stoi(value));
    else if (name == "align")
        instance->set_align((alignmask)std::stoi(value));
    else if (name == "flags")
        instance->flags().set((Widget::flag)std::stoi(value));
}

template <class T>
void set_widget_text_property(T instance, const std::string& name, const std::string& value)
{
    if (name == "text")
    {
        instance->set_text(value);
    }
    else if (name == "fontface")
    {
        Font font(instance->font());
        font.set_face(value);
        instance->set_font(font);
    }
    else if (name == "fontsize")
    {
        Font font(instance->font());
        font.set_size(std::stoi(value));
        instance->set_font(font);
    }
    else if (name == "fontweight")
    {
        Font font(instance->font());
        font.set_weight((Font::weightid)std::stoi(value));
        instance->set_font(font);
    }
}

template <class T>
void set_property(T instance, const std::string& name, const std::string& value)
{
    set_widget_property<T>(instance, name, value);
}

template <>
void set_property<Button*>(Button* instance, const std::string& name, const std::string& value)
{
    set_widget_property<Button*>(instance, name, value);
    set_widget_text_property<Button*>(instance, name, value);
}

template <>
void set_property<Label*>(Label* instance, const std::string& name, const std::string& value)
{
    set_widget_property<Label*>(instance, name, value);
    set_widget_text_property<Label*>(instance, name, value);
}

template <>
void set_property<TextBox*>(TextBox* instance, const std::string& name, const std::string& value)
{
    set_widget_property<TextBox*>(instance, name, value);
    set_widget_text_property<TextBox*>(instance, name, value);
}

template <class T>
static std::shared_ptr<Widget> create_widget(rapidxml::xml_node<>* node, std::shared_ptr<Frame> parent)
{
    auto instance = std::make_shared<T>();
    if (parent)
    {
        parent->add(instance);
    }

    auto name = node->first_attribute("name");
    if (name)
    {
        instance->set_name(name->value());
    }

    for (auto prop = node->first_node("property"); prop; prop = prop->next_sibling())
    {
        std::string pname = prop->first_attribute("name")->value();
        std::string pvalue = prop->value();

        set_property<T*>(instance.get(), pname, pvalue);
    }

    return std::static_pointer_cast<Widget>(instance);
}

using create_function =
    std::function<std::shared_ptr<Widget>(rapidxml::xml_node<>* widget, std::shared_ptr<Frame> parent)>;

static const std::map<std::string, create_function> allocators =
{
    {"Button", create_widget<Button>},
    {"Window", create_widget<Window>},
    {"Label", create_widget<Label>},
    {"TextBox", create_widget<TextBox>},
    {"StaticGrid", create_widget<StaticGrid>},
};

static std::shared_ptr<Widget> parse_widget(rapidxml::xml_node<>* node, std::shared_ptr<Frame> parent = nullptr)
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
