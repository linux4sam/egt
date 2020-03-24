/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_UILOADER_H
#define EGT_UILOADER_H

/**
 * @file
 * @brief UI XML loader.
 */

#include <egt/detail/meta.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
class Widget;

namespace experimental
{

/**
 * Parses and loads a UI XML file.
 *
 * @b Example
 * @code{.cpp}
 * egt::experimental::UiLoader loader;
 * auto window = loader.load("ui.xml");
 * window->show();
 * @endcode
 */
class EGT_API UiLoader
{
public:

    /**
     * Parses and loads UI XML and returns the parent Widget.
     *
     * @param uri URI to the XML to load.
     */
    virtual std::shared_ptr<Widget> load(const std::string& uri);
};

}
}
}

#endif
