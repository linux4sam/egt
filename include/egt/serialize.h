/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_SERIALIZE_H
#define EGT_SERIALIZE_H

/**
 * @file
 * @brief Serialize support for widgets.
 */

#include <egt/detail/meta.h>
#include <egt/widgetflags.h>
#include <list>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

namespace egt
{
inline namespace v1
{
class Widget;
class Image;
class Pattern;

/**
 * Abstract base serializer class.
 */
class EGT_API Serializer
{
public:
    using Context = void;

    Serializer() noexcept = default;
    Serializer(const Serializer&) = default;
    Serializer& operator=(const Serializer&) = default;
    Serializer(Serializer&&) noexcept = default;
    Serializer& operator=(Serializer&&) noexcept = default;

    /// Attributes array type.
    using Attributes = std::list<std::pair<std::string, std::string>>;

    using Properties = std::list<std::tuple<std::string, std::string, Serializer::Attributes>>;

    /// Add a widget to the serializer.
    virtual bool add(const Widget* widget) = 0;

    /// Create a new child.
    virtual Context* begin_child(const std::string& nodename) = 0;

    /// Complete a child.
    virtual void end_child(Context* context) = 0;

    /// Add a property.
    virtual void add_property(const std::string& name, const std::string& value,
                              const Attributes& attrs = {}) = 0;
    /// Add a property.
    void add_property(const std::string& name, const char* value,
                      const Attributes& attrs = {})
    {
        add_property(name, std::string(value), attrs);
    }

    /// Add a property.
    void add_property(const std::string& name, int value,
                      const Attributes& attrs = {});
    /// Add a property.
    void add_property(const std::string& name, unsigned int value,
                      const Attributes& attrs = {});
    /// Add a property.
    void add_property(const std::string& name, const AlignFlags& value,
                      const Attributes& attrs = {});
    /// Add a property.
    void add_property(const std::string& name, float value,
                      const Attributes& attrs = {});
    /// Add a property.
    void add_property(const std::string& name, double value,
                      const Attributes& attrs = {});
    /// Add a property.
    virtual void add_property(const std::string& name, const Pattern& value,
                              const Attributes& attrs = {}) = 0;

    /// Add a property.
    void add_property(const std::string& name, bool value,
                      const Attributes& attrs = {});

    /// Write to the specified ostream.
    virtual void write(std::ostream& out) = 0;

    virtual ~Serializer() noexcept = default;
};

/**
 * Serialize a widget tree to an std::ostream
 *
 * @code{.cpp}
 * // already have a Window variable named win
 * OstreamWidgetSerializer out(std::cout);
 * out.add(&win);
 * @endcode
 *
 * @see Widget::walk()
 */
class EGT_API OstreamWidgetSerializer : public Serializer
{
public:
    OstreamWidgetSerializer();
    OstreamWidgetSerializer(const OstreamWidgetSerializer&) = delete;
    OstreamWidgetSerializer& operator=(const OstreamWidgetSerializer&) = delete;
    OstreamWidgetSerializer(OstreamWidgetSerializer&&) noexcept = default;
    OstreamWidgetSerializer& operator=(OstreamWidgetSerializer&&) noexcept = default;

    /// Clear or reset, the serializer for re-use.
    void reset();

    bool add(const Widget* widget) override;

    Context* begin_child(const std::string& nodename) override;

    void end_child(Context* context) override;

    using Serializer::add_property;

    void add_property(const std::string& name, const std::string& value,
                      const Attributes& attrs = {}) override;

    void add_property(const std::string& name, const Pattern& value,
                      const Attributes& attrs = {}) override;

    void write(std::ostream& out) override;

    ~OstreamWidgetSerializer() noexcept override;

private:

    struct OstreamSerializerImpl;
    std::unique_ptr<OstreamSerializerImpl> m_impl;
};

/**
 * Serialize a widget tree to an XML document.
 *
 * @code{.cpp}
 * // already have a Window variable named win
 * XmlWidgetSerializer xml;
 * xml.add(&win);
 * // write the result
 * xml.write("output.xml");
 * // or
 * xml.write(std::cout);
 * @endcode
 *
 * @see Widget::walk()
 */
class EGT_API XmlWidgetSerializer : public Serializer
{
public:
    XmlWidgetSerializer();
    XmlWidgetSerializer(const XmlWidgetSerializer&) = delete;
    XmlWidgetSerializer& operator=(const XmlWidgetSerializer&) = delete;
    XmlWidgetSerializer(XmlWidgetSerializer&&) noexcept = default;
    XmlWidgetSerializer& operator=(XmlWidgetSerializer&&) noexcept = default;

    /// Clear or reset, the serializer for re-use.
    void reset();

    bool add(const Widget* widget) override;

    Context* begin_child(const std::string& nodename) override;

    void end_child(Context* context) override;

    using Serializer::add_property;

    void add_property(const std::string& name, const std::string& value,
                      const Attributes& attrs = {}) override;

    void add_property(const std::string& name, const Pattern& value,
                      const Attributes& attrs = {}) override;

    /// Write to the specified file path.
    void write(const std::string& filename);

    /// Write to the specified ostream.
    void write(std::ostream& out) override;

    ~XmlWidgetSerializer() noexcept override;

private:

    struct XmlSerializerImpl;
    std::unique_ptr<XmlSerializerImpl> m_impl;
};

class EGT_API Deserializer
{
public:
    virtual ~Deserializer() {}
    virtual bool is_valid() const = 0;
    virtual std::unique_ptr<Deserializer> first_child(const std::string& name = "") const = 0;
    virtual std::unique_ptr<Deserializer> next_sibling(const std::string& name = "") const = 0;
    virtual std::shared_ptr<Widget> parse_widget() const = 0;
    virtual bool get_property(const std::string& name, std::string* value, Serializer::Attributes* attrs = nullptr) const = 0;
};

}
}

#endif
