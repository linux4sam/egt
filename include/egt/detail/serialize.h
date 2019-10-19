/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SERIALIZE_H
#define EGT_DETAIL_SERIALIZE_H

/**
 * @file
 * @brief Serialize widgets.
 */

#include <egt/detail/meta.h>
#include <egt/widget.h>
#include <fstream>
#include <ostream>
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{
namespace detail
{

/**
 * Serialize a widget tree to an std::ostream
 *
 * @see Widget::walk()
 */
struct EGT_API OstreamWidgetSerializer
{
    explicit OstreamWidgetSerializer(std::ostream& o)
        : out(o)
    {}

    bool add(Widget* widget, int level)
    {
        out << std::string(level, ' ') << widget->name() <<
            " " << widget->box() << " " << widget->flags();
        out << " box(" << widget->margin() << "," <<
            widget->padding() << "," << widget->border() << ")";
        out << " align(" << static_cast<int>(widget->align()) << ")";
        out << std::endl;

        return true;
    }

    std::ostream& out;
};

/**
 * Serialize a widget tree to an XML document.
 *
 * @see Widget::walk()
 */
struct EGT_API XmlWidgetSerializer
{
    XmlWidgetSerializer()
    {
        reset();
    }

    void reset()
    {
        doc.clear();
        stack.clear();

        auto decl = doc.allocate_node(rapidxml::node_declaration);
        decl->append_attribute(doc.allocate_attribute("version", "1.0"));
        decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
        doc.append_node(decl);

        auto root = doc.allocate_node(rapidxml::node_element, "widgets");
        doc.append_node(root);
        stack.push_back(root);
    }

    template<class T>
    rapidxml::xml_node<>* create_node(const std::string& name, std::function<T()> func)
    {
        auto n = doc.allocate_string(name.c_str());
        auto node = doc.allocate_node(rapidxml::node_element, n);
        auto v = doc.allocate_string(std::to_string(func()).c_str());
        node->value(v);
        return node;
    }

    bool add(Widget* widget, int level)
    {
        if (level < static_cast<int>(stack.size()) - 1)
            stack.pop_back();

        auto child = doc.allocate_node(rapidxml::node_element, "widget");
        auto str = doc.allocate_string(widget->name().c_str());
        child->append_attribute(doc.allocate_attribute("name", str));
        str = doc.allocate_string("Widget");
        child->append_attribute(doc.allocate_attribute("type", str));
        str = doc.allocate_string("1.0");
        child->append_attribute(doc.allocate_attribute("version", str));

        child->append_node(create_node<int>("x", std::bind(&Widget::x, widget)));
        child->append_node(create_node<int>("y", std::bind(&Widget::y, widget)));
        child->append_node(create_node<int>("w", std::bind(&Widget::width, widget)));
        child->append_node(create_node<int>("h", std::bind(&Widget::height, widget)));
        child->append_node(create_node<int>("border", std::bind(&Widget::border, widget)));
        child->append_node(create_node<int>("margin", std::bind(&Widget::margin, widget)));
        child->append_node(create_node<int>("padding", std::bind(&Widget::padding, widget)));
        child->append_node(create_node<int>("xratio", std::bind(&Widget::xratio, widget)));
        child->append_node(create_node<int>("yratio", std::bind(&Widget::yratio, widget)));
        child->append_node(create_node<int>("horizontal_ratio", std::bind(&Widget::horizontal_ratio, widget)));
        child->append_node(create_node<int>("vertical_ratio", std::bind(&Widget::vertical_ratio, widget)));

        stack.back()->append_node(child);
        stack.push_back(child);

        return true;
    }

    void write(const std::string& filename)
    {
        std::ofstream file_stored(filename.c_str());
        file_stored << doc;
        file_stored.close();
    }

    void write(std::ostream& out)
    {
        out << doc;
    }

    rapidxml::xml_document<> doc;
    std::vector<rapidxml::xml_node<>*> stack;
};

}
}
}

#endif
