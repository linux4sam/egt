/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <detail/fmt.h>
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
    add_property(name, detail::to_string(value), attrs);
}

void Serializer::add_property(const std::string& name, double value,
                              const Attributes& attrs)
{
    add_property(name, detail::to_string(value), attrs);
}

void Serializer::add_property(const std::string& name, bool value,
                              const Attributes& attrs)
{
    add_property(name, detail::to_string(value), attrs);
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
    m_impl->current->value = "egt";
    m_impl->stack.push_back(m_impl->current);
}

void OstreamWidgetSerializer::previous_node()
{
    m_impl->current = m_impl->stack.back();
}

void OstreamWidgetSerializer::add_node(std::string nodename)
{
    m_level++;

    const auto advance = m_level > static_cast<int>(m_impl->stack.size()) - 1;
    if (advance)
    {
        m_impl->stack.push_back(m_impl->current);
    }
    else
    {
        while (static_cast<int>(m_impl->stack.size()) - 1 > m_level)
            m_impl->stack.pop_back();
    }

    m_impl->stack.back()->children.emplace_back();
    m_impl->current = &m_impl->stack.back()->children.back();
    m_impl->current->value = nodename;

    --m_level;
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

    if (m_level == 0)
    {
        m_impl->stack.back()->children.emplace_back();
        m_impl->current = &m_impl->stack.back()->children.back();

        //start with egt node
        m_impl->current->value = "egt";
        m_impl->stack.push_back(m_impl->current);
        previous_node();

        //Add global theme
        add_node("theme");
        std::ostringstream out;
        out << "type"  << "=" << global_theme().name();
        m_impl->current->attrs.push_back(out.str());
        previous_node();

        // Add global palette
        if (global_palette())
        {
            add_node("palette");
            global_palette()->serialize("color", *this);
            previous_node();
        }

        // Add global font
        if (global_font())
        {
            add_node("font");
            global_font()->serialize("font", *this);
            previous_node();
        }
    }

    m_impl->stack.back()->children.emplace_back();
    m_impl->current = &m_impl->stack.back()->children.back();
    m_impl->current->value = widget->name();
    m_impl->current->attrs.push_back("type=" + widget->type());

    widget->serialize(*this);

    return true;
}

void OstreamWidgetSerializer::add_property(const std::string& name,
        const std::string& value,
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

void OstreamWidgetSerializer::add_property(const std::string& name, const Pattern& value,
        const Attributes& attrs)
{
    std::ostringstream out;
    if (value.type() == Pattern::Type::solid)
    {
        out << name  << "=" << value.solid().hex().c_str();

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
    }
    else
    {
        out << name  << "=";
        if (value.type() == Pattern::Type::linear)
        {
            out << "pattern [type=linear";
        }
        else if (value.type() == Pattern::Type::linear_vertical)
        {
            out << "pattern [type=linear_vertical";
        }
        else if (value.type() == Pattern::Type::radial)
        {
            out << "pattern [type=radial";
        }

        out << " start=" << detail::to_string(value.starting());

        out << " end=" << detail::to_string(value.ending());

        auto steps = value.steps();
        if (!steps.empty())
        {
            size_t count = 0;
            out << " steps=[";
            for (auto& s : steps)
            {
                out << "{" << detail::to_string(s.first) << "," << s.second.hex().c_str() << "}";

                if (++count <= (steps.size() - 1))
                    out << ",";
            }
            out << "]";
        }

        if (value.type() == Pattern::Type::radial)
        {
            out << " start_radius=" << detail::to_string(value.starting_radius());
            out << " end_radius=" << detail::to_string(value.ending_radius());
        }

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
    }
    m_impl->current->attrs.push_back(out.str());
}

OstreamWidgetSerializer::~OstreamWidgetSerializer() noexcept = default;

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
        const bool l = (i + 1) == node.children.end();
        print_node(prefix, out, *i, l);
    }
}

void OstreamWidgetSerializer::write(std::ostream& out)
{
    if (m_impl->doc.children.empty())
        return;

    std::string prefix;
    print_node(prefix, out, m_impl->doc.children.front());
}

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

    auto theme = m_impl->doc.allocate_node(rapidxml::node_element, "theme");
    root->append_node(theme);
    auto node = m_impl->doc.allocate_node(rapidxml::node_element, "property");
    node->value(m_impl->doc.allocate_string(global_theme().name().c_str()));
    node->append_attribute(m_impl->doc.allocate_attribute("name",
                           m_impl->doc.allocate_string("type")));
    theme->append_node(node);

    if (global_palette())
    {
        m_impl->current = m_impl->doc.allocate_node(rapidxml::node_element, "palette");
        root->append_node(m_impl->current);
        m_impl->stack.push_back(m_impl->current);
        global_palette()->serialize("color", *this);
        m_impl->stack.pop_back();
    }

    if (global_font())
    {
        m_impl->current = m_impl->doc.allocate_node(rapidxml::node_element, "font");
        root->append_node(m_impl->current);
        m_impl->stack.push_back(m_impl->current);
        global_font()->serialize("font", *this);
        m_impl->stack.pop_back();
    }

    auto widgets = m_impl->doc.allocate_node(rapidxml::node_element, "widgets");
    root->append_node(widgets);
    m_impl->stack.push_back(widgets);
}

void XmlWidgetSerializer::add_node(std::string nodename)
{
    m_level++;

    const auto advance = m_level > static_cast<int>(m_impl->stack.size()) - 1;
    if (advance)
    {
        m_impl->stack.push_back(m_impl->current);
    }
    else
    {
        while (static_cast<int>(m_impl->stack.size()) - 1 > m_level)
            m_impl->stack.pop_back();
    }

    m_impl->current = m_impl->doc.allocate_node(rapidxml::node_element,
                      m_impl->doc.allocate_string(nodename.c_str()));
    m_impl->stack.back()->append_node(m_impl->current);

    --m_level;
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

void XmlWidgetSerializer::previous_node()
{
    m_impl->current = m_impl->stack.back();
}

void XmlWidgetSerializer::add_property(const std::string& name,
                                       const std::string& value,
                                       const Attributes& attrs)
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

void XmlWidgetSerializer::add_property(const std::string& name, const Pattern& value,
                                       const Attributes& attrs)
{
    auto node = m_impl->doc.allocate_node(rapidxml::node_element, "property");

    node->append_attribute(m_impl->doc.allocate_attribute("name",
                           m_impl->doc.allocate_string(name.c_str())));

    for (const auto& a : attrs)
    {
        node->append_attribute(m_impl->doc.allocate_attribute(m_impl->doc.allocate_string(a.first.c_str()),
                               m_impl->doc.allocate_string(a.second.c_str())));
    }

    if (value.type() == Pattern::Type::solid)
    {
        node->value(m_impl->doc.allocate_string(value.first().hex().c_str()));
        m_impl->current->append_node(node);
    }
    else
    {
        m_level++;

        const auto advance = m_level > static_cast<int>(m_impl->stack.size()) - 1;
        if (advance)
        {
            m_impl->stack.push_back(m_impl->current);
        }
        else
        {
            while (static_cast<int>(m_impl->stack.size()) - 1 > m_level)
                m_impl->stack.pop_back();
        }

        auto patternnode  = m_impl->doc.allocate_node(rapidxml::node_element, "pattern");
        node->append_node(patternnode);

        auto snode = m_impl->doc.allocate_node(rapidxml::node_element, "property");
        snode->append_attribute(m_impl->doc.allocate_attribute("name", "start"));
        snode->value(m_impl->doc.allocate_string(detail::to_string(value.starting()).c_str()));

        auto enode = m_impl->doc.allocate_node(rapidxml::node_element, "property");
        enode->append_attribute(m_impl->doc.allocate_attribute("name", "end"));
        enode->value(m_impl->doc.allocate_string(detail::to_string(value.ending()).c_str()));

        const auto& steps = value.steps();
        std::ostringstream oss;
        auto stepsnode = m_impl->doc.allocate_node(rapidxml::node_element, "property");
        stepsnode->append_attribute(m_impl->doc.allocate_attribute("name", "steps"));
        if (!steps.empty())
        {
            std::string tmp;
            for (const auto& s : steps)
            {
                tmp += "{" + detail::to_string(s.first) + "," + s.second.hex().c_str() + "},";
            }
            stepsnode->value(m_impl->doc.allocate_string(tmp.c_str()));
        }

        if (value.type() == Pattern::Type::linear)
        {
            auto pnode = m_impl->doc.allocate_node(rapidxml::node_element, "property");
            pnode->append_attribute(m_impl->doc.allocate_attribute("name", "type"));
            pnode->value("linear");

            patternnode->append_node(pnode);
            patternnode->append_node(snode);
            patternnode->append_node(enode);
            if (!steps.empty())
                patternnode->append_node(stepsnode);
        }
        else if (value.type() == Pattern::Type::linear_vertical)
        {
            auto pnode = m_impl->doc.allocate_node(rapidxml::node_element, "property");
            pnode->append_attribute(m_impl->doc.allocate_attribute("name", "type"));
            pnode->value("linear_vertical");

            patternnode->append_node(pnode);
            patternnode->append_node(snode);
            patternnode->append_node(enode);
            if (!steps.empty())
                patternnode->append_node(stepsnode);
        }
        else if (value.type() == Pattern::Type::radial)
        {
            auto rnode = m_impl->doc.allocate_node(rapidxml::node_element, "property");
            rnode->append_attribute(m_impl->doc.allocate_attribute("name", "type"));
            rnode->value("radial");

            patternnode->append_node(rnode);
            patternnode->append_node(snode);
            patternnode->append_node(enode);
            if (!steps.empty())
                patternnode->append_node(stepsnode);

            auto start_radius = detail::to_string(value.starting_radius());

            auto sradius = m_impl->doc.allocate_node(rapidxml::node_element, "property");
            sradius->append_attribute(m_impl->doc.allocate_attribute("name", "start_radius"));
            sradius->value(m_impl->doc.allocate_string(start_radius.c_str()));

            patternnode->append_node(sradius);

            auto end_radius = detail::to_string(value.ending_radius());

            auto eradius = m_impl->doc.allocate_node(rapidxml::node_element, "property");
            eradius->append_attribute(m_impl->doc.allocate_attribute("name", "end_radius"));
            eradius->value(m_impl->doc.allocate_string(end_radius.c_str()));
            patternnode->append_node(eradius);
        }
        --m_level;
        m_impl->current->append_node(node);
        m_impl->stack.pop_back();
    }
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
