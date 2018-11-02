/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "mui/ui"
#include "mui/uiloader.h"

#ifdef HAVE_RAPIDXML_RAPIDXML_HPP
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#else
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#endif

using namespace std;

namespace mui
{
    namespace experimental
    {
        template <class T>
        void set_widget_property(T instance, const string& name, const string& value)
        {
            if (name == "width")
                instance->width(std::stoi(value));
            else if (name == "height")
                instance->height(std::stoi(value));
            else if (name == "x")
                instance->movex(std::stoi(value));
            else if (name == "y")
                instance->movey(std::stoi(value));
            else if (name == "align")
                instance->align((alignmask)std::stoi(value));
            else if (name == "flags")
                instance->flag_set((widgetmask)std::stoi(value));
        }

        template <class T>
        void set_widget_text_property(T instance, const string& name, const string& value)
        {
            if (name == "text")
            {
                instance->text(value);
            }
            else if (name == "fontface")
            {
                Font font(instance->font());
                font.face(value);
                instance->font(font);
            }
            else if (name == "fontsize")
            {
                Font font(instance->font());
                font.size(std::stoi(value));
                instance->font(font);
            }
            else if (name == "fontweight")
            {
                Font font(instance->font());
                font.weight((Font::weightid)std::stoi(value));
                instance->font(font);
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

        using create_function = std::function<Widget*(rapidxml::xml_node<>* widget, Frame* parent)>;

        static const map<string, create_function> allocators =
        {
            {"Button", create_widget<Button>},
            {"Window", create_widget<Window>},
            {"Label", create_widget<Label>},
            {"TextBox", create_widget<TextBox>},
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
