/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#ifndef EGT_UILOADER_H
#define EGT_UILOADER_H

/**
 * @file
 * @brief UI XML loader.
 */

#include <string>

namespace egt
{
    class Widget;

    namespace experimental
    {
        class UiLoader
        {
        public:

            UiLoader();

            virtual Widget* load(const std::string& file);

            virtual ~UiLoader();
        };
    }
}

#endif
