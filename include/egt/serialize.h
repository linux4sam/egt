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

    /// Attributes array type.
    using Attributes = std::vector<std::pair<std::string, std::string>>;

    /// Add a widget to the serializer.
    virtual bool add(const Widget* widget, int level = 0) = 0;

    /// Add a property.
    void add_property(const std::string& name, const std::string& value,
                      const Attributes& attrs = {})
    {
        add_property(name, value.c_str(), attrs);
    }

    /// Add a property.
    virtual void add_property(const std::string& name, const char* value,
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

    /// Add a property.
    virtual void add_property(const std::string& name, bool value);

    /// Get the current level
    int level() const { return m_level; }

    /// Write to the specified ostream.
    virtual void write(std::ostream& out) = 0;

protected:

    /// Current serialize tree level
    int m_level{0};
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
    EGT_OPS_NOCOPY_MOVE(OstreamWidgetSerializer);
    ~OstreamWidgetSerializer() noexcept;

    /// Clear or reset, the serializer for re-use.
    void reset();

    bool add(const Widget* widget, int level = 0) override;

    using Serializer::add_property;

    void add_property(const std::string& name, const char* value,
                      const Attributes& attrs = {}) override;

    void write(std::ostream& out) override;

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
    EGT_OPS_NOCOPY_MOVE(XmlWidgetSerializer);
    ~XmlWidgetSerializer() noexcept;

    /// Clear or reset, the serializer for re-use.
    void reset();

    bool add(const Widget* widget, int level = 0) override;

    using Serializer::add_property;

    void add_property(const std::string& name, const char* value,
                      const Attributes& attrs = {}) override;


    /// Write to the specified file path.
    void write(const std::string& filename);

    /// Write to the specified ostream.
    void write(std::ostream& out) override;

private:

    struct XmlSerializerImpl;
    std::unique_ptr<XmlSerializerImpl> m_impl;
};

}
}

#endif
