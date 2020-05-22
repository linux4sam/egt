/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/color.h>
#include <egt/detail/string.h>
#include <egt/font.h>
#include <egt/image.h>
#include <egt/serialize.h>
#include <egt/widget.h>
#include <fstream>
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>
#include <vector>

namespace egt
{
inline namespace v1
{

void Serializer::add_property(const std::string& name, int value,
                              const Attributes& attrs)
{
    add_property(name, std::to_string(value), attrs);
}

void Serializer::add_property(const std::string& name, unsigned int value,
                              const Attributes& attrs)
{
    add_property(name, std::to_string(value), attrs);
}

void Serializer::add_property(const std::string& name, const AlignFlags& value,
                              const Attributes& attrs)
{
    add_property(name, value.to_string(), attrs);
}

void Serializer::add_property(const std::string& name, float value,
                              const Attributes& attrs)
{
    add_property(name, std::to_string(value), attrs);
}

void Serializer::add_property(const std::string& name, double value,
                              const Attributes& attrs)
{
    add_property(name, std::to_string(value), attrs);
}

void Serializer::add_property(const std::string& name, const Pattern& value)
{
    // TODO: only works with a single color
    auto color = value.color();
    add_property(name, color.pixel32());
}

void Serializer::add_property(const std::string& name, bool value)
{
    add_property(name, detail::to_string(value));
}

template<class T>
struct Node
{
    void clear()
    {
        value.clear();
        attrs.clear();
        children.clear();
    }

    T value;
    std::vector<std::string> attrs;
    std::vector<Node<T>> children;
};

using StringNode = Node<std::string>;

struct OstreamWidgetSerializer::OstreamSerializerImpl
{
    StringNode doc;
    std::vector<StringNode*> stack;
    StringNode* current{nullptr};
};

OstreamWidgetSerializer::OstreamWidgetSerializer()
    : m_impl(std::make_unique<OstreamSerializerImpl>())
{
    reset();
}

void OstreamWidgetSerializer::reset()
{
    m_level = 0;
    m_impl->current = &m_impl->doc;
    m_impl->doc.clear();
    m_impl->stack.clear();
    m_impl->stack.push_back(m_impl->current);
}

bool OstreamWidgetSerializer::add(const Widget* widget, int level)
{
    m_level = level;

    const auto advance = level > static_cast<int>(m_impl->stack.size()) - 1;
    if (advance)
    {
        m_impl->stack.push_back(m_impl->current);
    }
    else
    {
        while (static_cast<int>(m_impl->stack.size()) - 1 > level)
            m_impl->stack.pop_back();
    }

    m_impl->stack.back()->children.emplace_back();
    m_impl->current = &m_impl->stack.back()->children.back();
    m_impl->current->value = widget->name();
    m_impl->current->attrs.push_back("type=" + widget->type());

    widget->serialize(*this);

    return true;
}

void OstreamWidgetSerializer::add_property(const std::string& name,
        const char* value,
        const Attributes& attrs)
{
    std::ostringstream out;
    out << name  << "=" << value;

    if (!attrs.empty())
    {
        out << " [";
        for (auto i = attrs.begin(); i != attrs.end(); ++i)
        {
            if (i != attrs.begin())
                out << " ";
            out << i->first << "=" << i->second;
        }
        out << "]";
    }

    m_impl->current->attrs.push_back(out.str());
}

static void print_node(std::string prefix, std::ostream& out,
                       const StringNode& node, bool last = true)
{
    out << prefix;
    if (last)
        out << "┕";
    else
        out << "┝";
    out << node.value << "\n";

    if (last)
        prefix += " ";
    else
        prefix += "│";

    for (auto i = node.attrs.begin(); i != node.attrs.end(); ++i)
    {
        out << prefix;

        if (!node.children.empty())
            out << "│";

        out << " " << *i << "\n";
    }

    for (auto i = node.children.begin(); i != node.children.end(); ++i)
    {
        const bool last = (i + 1) == node.children.end();
        print_node(prefix, out, *i, last);
    }
}

void OstreamWidgetSerializer::write(std::ostream& out)
{
    if (m_impl->doc.children.empty())
        return;

    std::string prefix;
    print_node(prefix, out, m_impl->doc.children.front());
}

OstreamWidgetSerializer::~OstreamWidgetSerializer() noexcept = default;

struct XmlWidgetSerializer::XmlSerializerImpl
{
    rapidxml::xml_document<> doc;
    std::vector<rapidxml::xml_node<>*> stack;
    rapidxml::xml_node<>* current{nullptr};
};

XmlWidgetSerializer::XmlWidgetSerializer()
    : m_impl(std::make_unique<XmlSerializerImpl>())
{
    reset();
}

void XmlWidgetSerializer::reset()
{
    m_level = 0;
    m_impl->current = nullptr;
    m_impl->doc.clear();
    m_impl->stack.clear();

    auto decl = m_impl->doc.allocate_node(rapidxml::node_declaration);
    decl->append_attribute(m_impl->doc.allocate_attribute("version", "1.0"));
    decl->append_attribute(m_impl->doc.allocate_attribute("encoding", "utf-8"));
    m_impl->doc.append_node(decl);

    auto root = m_impl->doc.allocate_node(rapidxml::node_element, "egt");
    m_impl->doc.append_node(root);

    auto widgets = m_impl->doc.allocate_node(rapidxml::node_element, "widgets");
    root->append_node(widgets);
    m_impl->stack.push_back(widgets);
}

bool XmlWidgetSerializer::add(const Widget* widget, int level)
{
    m_level = level;

    const auto advance = level > static_cast<int>(m_impl->stack.size()) - 1;
    if (advance)
    {
        m_impl->stack.push_back(m_impl->current);
    }
    else
    {
        while (static_cast<int>(m_impl->stack.size()) - 1 > level)
            m_impl->stack.pop_back();
    }

    m_impl->current = m_impl->doc.allocate_node(rapidxml::node_element, "widget");

    m_impl->current->append_attribute(m_impl->doc.allocate_attribute("name",
                                      m_impl->doc.allocate_string(widget->name().c_str())));
    m_impl->current->append_attribute(m_impl->doc.allocate_attribute("type",
                                      m_impl->doc.allocate_string(widget->type().c_str())));

    m_impl->stack.back()->append_node(m_impl->current);

    widget->serialize(*this);

    return true;
}

void XmlWidgetSerializer::add_property(const std::string& name,
                                       const char* value,
                                       const Attributes& attrs)
{
    auto node = m_impl->doc.allocate_node(rapidxml::node_element, "property");
    node->value(m_impl->doc.allocate_string(value));
    node->append_attribute(m_impl->doc.allocate_attribute("name",
                           m_impl->doc.allocate_string(name.c_str())));

    for (const auto& a : attrs)
    {
        node->append_attribute(m_impl->doc.allocate_attribute(m_impl->doc.allocate_string(a.first.c_str()),
                               m_impl->doc.allocate_string(a.second.c_str())));
    }

    m_impl->current->append_node(node);
}

void XmlWidgetSerializer::write(const std::string& filename)
{
    std::ofstream out(filename.c_str());
    write(out);
    out.close();
}

void XmlWidgetSerializer::write(std::ostream& out)
{
    out << m_impl->doc;
}

XmlWidgetSerializer::~XmlWidgetSerializer() noexcept = default;

}
}
