/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_DETAIL_SERIALIZE_H
#define EGT_DETAIL_SERIALIZE_H

/**
 * @file
 * @brief Serialize support for widgets.
 */

#include <egt/detail/meta.h>
#include <egt/theme.h>
#include <egt/widgetflags.h>
#include <map>
#include <memory>
#include <ostream>
#include <string>

namespace egt
{
inline namespace v1
{
class Widget;
class Image;
class Pattern;

namespace detail
{

/**
 * Base serializer class.
 */
class EGT_API Serializer
{
public:
    virtual bool add(Widget* widget, int level) = 0;

    virtual void add_property(const std::string& name, const std::string& value,
                              const std::map<std::string, std::string>& attrs = {}) = 0;
    virtual void add_property(const std::string& name, int value,
                              const std::map<std::string, std::string>& attrs = {});
    virtual void add_property(const std::string& name, unsigned int value,
                              const std::map<std::string, std::string>& attrs = {});
    virtual void add_property(const std::string& name, const AlignFlags& value,
                              const std::map<std::string, std::string>& attrs = {});
    virtual void add_property(const std::string& name, float value,
                              const std::map<std::string, std::string>& attrs = {});
    virtual void add_property(const std::string& name, double value,
                              const std::map<std::string, std::string>& attrs = {});
    virtual void add_property(const std::string& name, Theme::BoxFlags value);
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
    explicit OstreamWidgetSerializer(std::ostream& o);

    bool add(Widget* widget, int level) override;

    using Serializer::add_property;

    void add_property(const std::string& name, const std::string& value,
                      const std::map<std::string, std::string>& attrs = {}) override;

protected:
    std::ostream& m_out;
    int m_level{0};
};

/**
 * Serialize a widget tree to an XML document.
 *
 * @see Widget::walk()
 */
class EGT_API XmlWidgetSerializer : public Serializer
{
public:
    XmlWidgetSerializer();

    void reset();

    bool add(Widget* widget, int level) override;

    using Serializer::add_property;

    void add_property(const std::string& name, const std::string& value,
                      const std::map<std::string, std::string>& attrs = {}) override;

    void write(const std::string& filename);
    void write(std::ostream& out);

    virtual ~XmlWidgetSerializer() noexcept;

protected:

    struct XmlSerializerImpl;
    std::unique_ptr<XmlSerializerImpl> m_impl;
};

}
}
}

#endif
