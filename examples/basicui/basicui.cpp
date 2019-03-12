/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/detail/imagecache.h>
#include <egt/ui>
#include <egt/uiloader.h>

int main(int argc, const char** argv)
{
    egt::Application app(argc, argv);

    egt::experimental::UiLoader loader;
    auto window = loader.load(egt::detail::resolve_file_path("ui.xml"));
    window->show();

    return app.run();
}
