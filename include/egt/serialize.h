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
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

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
    using Attributes = std::vector<std::pair<std::string, std::string>>;

    /// Add a widget to the serializer.
    virtual bool add(Widget* widget, int level) = 0;
    /// Add a property.
    virtual void add_property(const std::string& name, const std::string& value,
                              const Attributes& attrs = {}) = 0;
    /// Add a property.
    virtual void add_property(const std::string& name, int value,
                              const Attributes& attrs = {});
    /// Add a property.
    virtual void add_property(const std::string& name, unsigned int value,
                              const Attributes& attrs = {});
    /// Add a property.
    virtual void add_property(const std::string& name, const AlignFlags& value,
                              const Attributes& attrs = {});
    /// Add a property.
    virtual void add_property(const std::string& name, float value,
                              const Attributes& attrs = {});
    /// Add a property.
    virtual void add_property(const std::string& name, double value,
                              const Attributes& attrs = {});
    /// Add a property.
    virtual void add_property(const std::string& name, const Pattern& value);
};

/**
 * Serialize a widget tree to an std::ostream
 *
 * @see Widget::walk()
 */
class EGT_API OstreamWidgetSerializer : public Serializer
{
public:
    /**
     * @param o Output stream reference.
     */
    explicit OstreamWidgetSerializer(std::ostream& o);

    virtual bool add(Widget* widget, int level) override;

    using Serializer::add_property;

    void add_property(const std::string& name, const std::string& value,
                      const Attributes& attrs = {}) override;

protected:
    /// Output stream reference
    std::ostream& m_out;

    /// Current serialize tree level
    int m_level{0};
};

/**
 * Serialize a widget tree to an XML document.
 *
 * @code{.cpp}
 * // already have a Window variable named win
 * XmlWidgetSerializer xml;
 * win.walk(std::bind(&XmlWidgetSerializer::add, std::ref(xml),
 *                    std::placeholders::_1, std::placeholders::_2));
 * // write the result
 * xml.write("output.xml");
 * // or
 * xml.write(std::cout);
 *@endcode
 *
 * @see Widget::walk()
 */
class EGT_API XmlWidgetSerializer : public Serializer
{
public:
    XmlWidgetSerializer();

    /// Clear or reset, the serializer for re-use.
    void reset();

    virtual bool add(Widget* widget, int level) override;

    using Serializer::add_property;

    void add_property(const std::string& name, const std::string& value,
                      const Attributes& attrs = {}) override;


    /// Write top the specified file path.
    void write(const std::string& filename);

    /// Write to the specified ostream.
    void write(std::ostream& out);

    virtual ~XmlWidgetSerializer() noexcept;

protected:

    struct XmlSerializerImpl;

    /// @private
    std::unique_ptr<XmlSerializerImpl> m_impl;
};

}
}

#endif
