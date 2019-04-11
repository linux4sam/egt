/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cmath>
#include <cstdio>
#include <egt/detail/filesystem.h>
#include <egt/ui>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

#define SHARED_PATH detail::exe_pwd() + "/../share/egt/examples/gallery/"

int main(int argc, const char** argv)
{
    Application app(argc, argv, "gallery");

    std::vector<std::string> files = detail::glob(SHARED_PATH + "*trailer*.png");

    TopWindow win;
    win.set_name("win");
    win.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::black);

    SoftwareVideo player(Size(320, 192));
    player.set_volume(50);

    auto fullscale = (double)main_screen()->size().w / (double)player.w();
    if (fullscale <= 0)
        fullscale = 1.0;

    ImageLabel logo(Image("@microchip_logo_white.png"));
    win.add(logo);
    logo.set_align(alignmask::left | alignmask::top, 10);

    auto grid_height = (win.size().h - logo.h()) / 2;

    ScrolledView view0(Rect(0, logo.h(), win.size().w, grid_height));
    view0.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::black);
    view0.set_name("view0");
    win.add(view0);

    StaticGrid grid0(Rect(0, 0, files.size() * 150, grid_height), Tuple(files.size(), 1));
    grid0.set_name("grid0");
    view0.add(grid0);

    PropertyAnimatorType<float> animator(1.0, fullscale, std::chrono::milliseconds(800));
    bool videoshown = false;

    for (auto& file : files)
    {
        auto l = make_shared<ImageButton>(Image(file));
        l->set_boxtype(Theme::boxtype::none);
        l->flags().clear(Widget::flag::grab_mouse);
        l->on_event([&player, file, &animator, &videoshown](eventid)
        {
            cout << "playing " << file << ".avi" << endl;
            player.set_media(file + ".avi");
            player.play();
            player.show();
            videoshown = true;

            animator.clear_change_callbacks();
            animator.on_change(std::bind(&SoftwareVideo::set_scale, std::ref(player), std::placeholders::_1));
            animator.start();

            return 0;
        }, {eventid::pointer_click});

        l->set_align(alignmask::center);
        grid0.add(l);
    }

    ScrolledView view1(Rect(0, logo.h() + grid_height + 1, win.size().w, grid_height));
    view1.palette().set(Palette::ColorId::bg, Palette::GroupId::normal, Palette::black);
    view1.set_name("view1");
    win.add(view1);

    StaticGrid grid1(Rect(0, 0, files.size() * 150, grid_height), Tuple(files.size(), 1));
    grid1.set_name("grid1");
    view1.add(grid1);

    for (auto& file : files)
    {
        auto l = make_shared<ImageButton>(Image(file));
        l->set_boxtype(Theme::boxtype::none);
        l->flags().clear(Widget::flag::grab_mouse);
        l->on_event([&player, file, &animator, &videoshown](eventid)
        {
            cout << "playing " << file << ".avi" << endl;
            player.set_media(file + ".avi");
            player.play();
            player.show();
            videoshown = true;

            animator.clear_change_callbacks();
            animator.on_change(std::bind(&SoftwareVideo::set_scale, std::ref(player), std::placeholders::_1));
            animator.start();

            return 0;
        }, {eventid::pointer_click});

        l->set_align(alignmask::center);
        grid1.add(l);
    }

    Popup popup(Size(main_screen()->size().w / 2, main_screen()->size().h / 2));
    popup.set_name("popup");
    Button button("Hello World");
    popup.add(button);
    button.set_align(alignmask::center);
    button.set_name("hw");

    ImageButton settings(Image("settings.png"), "", Rect());
    win.add(settings);
    settings.set_boxtype(Theme::boxtype::none);
    settings.set_align(alignmask::right | alignmask::top, 10);
    settings.on_event([&popup](eventid)
    {
        if (popup.visible())
            popup.hide();
        else
            popup.show(true);
        return 1;
    }, {eventid::pointer_click});
    win.add(popup);

    win.add(player);

    win.show();

    Input::global_input().on_event([&player, &videoshown](eventid)
    {
        if (videoshown)
        {
            player.pause();
            player.hide();
            videoshown = false;
            return 1;
        }
        return 0;
    }, {eventid::pointer_click});

    return app.run();
}
