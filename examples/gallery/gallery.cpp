/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include <cmath>
#include <cstdio>
#include <glob.h>
#include <iostream>
#include <map>
#include <memory>
#include <mui/ui>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <string>
#include <vector>

using namespace std;
using namespace mui;

static std::vector<std::string> glob(const std::string& pattern)
{
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));

    int return_value = glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
    if (return_value != 0)
    {
        globfree(&glob_result);
        stringstream ss;
        ss << "glob() failed: " << return_value << endl;
        throw std::runtime_error(ss.str());
    }

    vector<string> filenames;
    for (size_t i = 0; i < glob_result.gl_pathc; ++i)
        filenames.push_back(string(glob_result.gl_pathv[i]));

    globfree(&glob_result);

    return filenames;
}

#define SHARED_PATH "../share/mui/examples/gallery/"

int main()
{
    Application app;

    std::vector<std::string> files = glob(SHARED_PATH "*.png");

    Window win;
    win.flag_set(widgetmask::NO_BORDER);
    win.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);

    Image logo(SHARED_PATH "logo.png");
    win.add(&logo);
    logo.align(alignmask::LEFT | alignmask::TOP, 10);

    ScrolledView view(Point(0, logo.h()), Size(win.size().w, win.size().h - logo.h()));
    view.flag_set(widgetmask::NO_BACKGROUND);
    win.add(&view);

    /** @todo This is not respective parent coordinate for origin point. */
    StaticGrid grid(Point(0, logo.h()), Size(files.size() / 2 * 150, view.h()), files.size() / 2, 2, 10);
    view.add(&grid);

    int column = 0;
    int row = 0;
    for (auto& file : files)
    {
        if (file.find("logo.png") != string::npos)
            continue;

        if (file.find("settings.png") != string::npos)
            continue;

        int r = (row++) % 2;

        auto image = new Image(file);
        grid.add(image, column, r, alignmask::CENTER);

        if (r != 0)
            column++;
    }

    Popup<Window> popup(Size(main_screen()->size().w / 2, main_screen()->size().h / 2));
    win.add(&popup);

    ImageButton settings(SHARED_PATH "settings.png", "", Point(), Size(), widgetmask::NO_BORDER);
    win.add(&settings);
    settings.align(alignmask::RIGHT | alignmask::TOP, 10);
    settings.add_handler([&popup](EventWidget * widget, int event)
    {
        ignoreparam(widget);

        if (event == EVT_MOUSE_DOWN)
        {
            if (popup.visible())
                popup.hide();
            else
                popup.show(true);
        }
    });

    win.show();

    return app.run();
}
