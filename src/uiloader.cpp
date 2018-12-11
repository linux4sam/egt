/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "egt/ui"
#include "egt/uiloader.h"

#ifdef HAVE_RAPIDXML_RAPIDXML_HPP
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#else
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#endif

using namespace std;

namespace egt
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
                instance->flag_set((widgetmask)std::stoi(value));
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
        static Widget* create_widget(rapidxml::xml_node<>* node, Frame* parent)
        {
            auto instance = new T;
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
                string name = prop->first_attribute("name")->value();
                string value = prop->first_attribute("value")->value();

                set_property<T*>(instance, name, value);
            }

            return instance;
        }
#if 0
        template <>
        static Widget* create_widget<StaticGrid*>(rapidxml::xml_node<>* node, Frame* parent)
        {
            auto instance = new StaticGrid;

            if (parent)
            {
                parent->add(instance);
            }

            auto row = node->first_attribute("row");
            auto column = node->first_attribute("column");
            if (row || column)
            {
                parent->add(instance, std::stoi(row), std::stoi(column));
            }
            else
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
                string name = prop->first_attribute("name")->value();
                string value = prop->first_attribute("value")->value();

                set_property<T*>(instance, name, value);
            }

            return instance;
        }
#endif
        using create_function = std::function<Widget*(rapidxml::xml_node<>* widget, Frame* parent)>;

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

        static Widget* parse_widget(rapidxml::xml_node<>* node, Frame* parent = nullptr)
        {
            Widget* result = nullptr;
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
                parse_widget(child, dynamic_cast<Frame*>(result));
            }

            return result;
        }

        Widget* UiLoader::load(const std::string& file)
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

            return nullptr;
        }

        UiLoader::~UiLoader()
        {}

    }

}
