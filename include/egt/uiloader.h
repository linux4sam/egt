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

#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{
class Widget;

namespace experimental
{
class UiLoader
{
public:

    UiLoader();

    virtual std::shared_ptr<Widget> load(const std::string& file);

    virtual ~UiLoader();
};
}

}
}

#endif
