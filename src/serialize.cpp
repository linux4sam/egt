/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/color.h>
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
                              const std::map<std::string, std::string>& attrs)
{
    add_property(name, std::to_string(value), attrs);
}

void Serializer::add_property(const std::string& name, unsigned int value,
                              const std::map<std::string, std::string>& attrs)
{
    add_property(name, std::to_string(value), attrs);
}

void Serializer::add_property(const std::string& name, const AlignFlags& value,
                              const std::map<std::string, std::string>& attrs)
{
    add_property(name, value.to_string(), attrs);
}

void Serializer::add_property(const std::string& name, float value,
                              const std::map<std::string, std::string>& attrs)
{
    add_property(name, std::to_string(value), attrs);
}

void Serializer::add_property(const std::string& name, double value,
                              const std::map<std::string, std::string>& attrs)
{
    add_property(name, std::to_string(value), attrs);
}

void Serializer::add_property(const std::string& name, const Theme::FillFlags& value)
{
    add_property(name, value.to_string());
}

void Serializer::add_property(const std::string& name, const Pattern& value)
{
    // TODO: only works with a single color
    auto color = value.color();
    add_property(name, color.pixel32());
}

OstreamWidgetSerializer::OstreamWidgetSerializer(std::ostream& o)
    : m_out(o)
{}

bool OstreamWidgetSerializer::add(Widget* widget, int level)
{
    m_level = level;

    m_out << std::endl;
    m_out << std::string(m_level, '+') << widget->name() << std::endl <<
          std::string(m_level + 1, ' ') << "type=" << widget->type();

    widget->serialize(*this);

    return true;
}

void OstreamWidgetSerializer::add_property(const std::string& name,
        const std::string& value,
        const std::map<std::string, std::string>& attrs)
{
    m_out << std::endl << std::string(m_level + 1, ' ') << name << "=" << value;

    if (!attrs.empty())
    {
        m_out << " [";
        bool first = true;
        for (auto& a : attrs)
        {
            if (first)
                first = false;
            else
                m_out << " ";
            m_out << a.first << "=" << a.second;
        }
        m_out << "]";
    }

    m_out << " ";
}

struct XmlWidgetSerializer::XmlSerializerImpl
{
    rapidxml::xml_document<> doc;
    std::vector<rapidxml::xml_node<>*> stack;
    rapidxml::xml_node<>* current{nullptr};
    int level{0};
};

XmlWidgetSerializer::XmlWidgetSerializer()
    : m_impl(new XmlSerializerImpl)
{
    reset();
}

void XmlWidgetSerializer::reset()
{
    m_impl->level = 0;
    m_impl->current = nullptr;
    m_impl->doc.clear();
    m_impl->stack.clear();

    auto decl = m_impl->doc.allocate_node(rapidxml::node_declaration);
    decl->append_attribute(m_impl->doc.allocate_attribute("version", "1.0"));
    decl->append_attribute(m_impl->doc.allocate_attribute("encoding", "utf-8"));
    m_impl->doc.append_node(decl);

    auto root = m_impl->doc.allocate_node(rapidxml::node_element, "widgets");
    m_impl->doc.append_node(root);
    m_impl->stack.push_back(root);
}

bool XmlWidgetSerializer::add(Widget* widget, int level)
{
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
    auto str = m_impl->doc.allocate_string(widget->name().c_str());
    m_impl->current->append_attribute(m_impl->doc.allocate_attribute("name", str));
    str = m_impl->doc.allocate_string(widget->type().c_str());
    m_impl->current->append_attribute(m_impl->doc.allocate_attribute("type", str));

    widget->serialize(*this);

    m_impl->stack.back()->append_node(m_impl->current);

    return true;
}

void XmlWidgetSerializer::add_property(const std::string& name,
                                       const std::string& value,
                                       const std::map<std::string, std::string>& attrs)
{
    auto node = m_impl->doc.allocate_node(rapidxml::node_element, "property");
    node->value(m_impl->doc.allocate_string(value.c_str()));
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
