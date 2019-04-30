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

using namespace std;

namespace egt
{
inline namespace v1
{
namespace experimental
{
template <class T>
void set_widget_property(T instance, const string& name, const string& value)
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
void set_widget_text_property(T instance, const string& name, const string& value)
{
    if (name == "text")
    {
        instance->set_text(value);
    }
    else if (name == "fontface")
    {
        Font font(instance->font());
        font.face(value);
        instance->set_font(font);
    }
    else if (name == "fontsize")
    {
        Font font(instance->font());
        font.size(std::stoi(value));
        instance->set_font(font);
    }
    else if (name == "fontweight")
    {
        Font font(instance->font());
        font.weight((Font::weightid)std::stoi(value));
        instance->set_font(font);
    }
}

template <class T>
void set_property(T instance, const string& name, const string& value)
{
    set_widget_property<T>(instance, name, value);
}

template <>
void set_property<Button*>(Button* instance, const string& name, const string& value)
{
    set_widget_property<Button*>(instance, name, value);
    set_widget_text_property<Button*>(instance, name, value);
}

template <>
void set_property<Label*>(Label* instance, const string& name, const string& value)
{
    set_widget_property<Label*>(instance, name, value);
    set_widget_text_property<Label*>(instance, name, value);
}

template <>
void set_property<TextBox*>(TextBox* instance, const string& name, const string& value)
{
    set_widget_property<TextBox*>(instance, name, value);
    set_widget_text_property<TextBox*>(instance, name, value);
}

template <class T>
static shared_ptr<Widget> create_widget(rapidxml::xml_node<>* node, shared_ptr<Frame> parent)
{
    auto instance = make_shared<T>();
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
        string pname = prop->first_attribute("name")->value();
        string pvalue = prop->value();

        set_property<T*>(instance.get(), pname, pvalue);
    }

    return static_pointer_cast<Widget>(instance);
}

using create_function = std::function<shared_ptr<Widget>(rapidxml::xml_node<>* widget, shared_ptr<Frame> parent)>;

static const map<string, create_function> allocators =
{
    {"Button", create_widget<Button>},
    {"Window", create_widget<Window>},
    {"Label", create_widget<Label>},
    {"TextBox", create_widget<TextBox>},
    {"StaticGrid", create_widget<StaticGrid>},
};

UiLoader::UiLoader()
{}

static shared_ptr<Widget> parse_widget(rapidxml::xml_node<>* node, shared_ptr<Frame> parent = nullptr)
{
    shared_ptr<Widget> result;
    string ttype;

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
        ERR("widget type " << type << " unsupported");
    }

    for (auto child = node->first_node("widget"); child; child = child->next_sibling())
    {
        parse_widget(child, dynamic_pointer_cast<Frame>(result));
    }

    return result;
}

shared_ptr<Widget> UiLoader::load(const std::string& file)
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

    return shared_ptr<Widget>();
}

UiLoader::~UiLoader()
{}

}

}
}
