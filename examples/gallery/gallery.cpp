/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cmath>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <egt/ui>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

#define SHARED_PATH detail::exe_pwd() + "/../share/egt/examples/gallery/"

int main(int argc, const char** argv)
{
    Application app(argc, argv, "gallery");

    std::vector<std::string> files = experimental::glob(SHARED_PATH + "*trailer*.png");

    TopWindow win;
    win.set_name("win");
    win.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);

    SoftwareVideo player(Size(320, 192));
    player.set_volume(50);

    auto fullscale = (double)main_screen()->size().w / (double)player.w();
    if (fullscale <= 0)
        fullscale = 1.0;

    ImageLabel logo(Image("@microchip_logo_white.png"));
    win.add(&logo);
    logo.set_align(alignmask::LEFT | alignmask::TOP, 10);

    auto grid_height = (win.size().h - logo.h()) / 2;

    ScrolledView view0(Rect(0, logo.h(), win.size().w, grid_height));
    view0.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    view0.set_name("view0");
    win.add(&view0);

    StaticGrid grid0(Rect(0, 0, files.size() * 150, grid_height), files.size(), 1, 0);
    grid0.set_name("grid0");
    view0.add(&grid0);

    PropertyAnimatorType<float> animator(1.0, fullscale, std::chrono::milliseconds(800));
    bool videoshown = false;

    for (auto& file : files)
    {
        auto l = new ImageButton(Image(file));
        l->set_boxtype(Theme::boxtype::none);
        l->clear_flag(widgetflag::GRAB_MOUSE);
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
        }, {eventid::POINTER_CLICK});

        grid0.add(l, alignmask::CENTER);
    }

    ScrolledView view1(Rect(0, logo.h() + grid_height + 1, win.size().w, grid_height));
    view1.palette().set(Palette::BG, Palette::GROUP_NORMAL, Color::BLACK);
    view1.set_name("view1");
    win.add(&view1);

    StaticGrid grid1(Rect(0, 0, files.size() * 150, grid_height), files.size(), 1, 0);
    grid1.set_name("grid1");
    view1.add(&grid1);

    for (auto& file : files)
    {
        auto l = new ImageButton(Image(file));
        l->set_boxtype(Theme::boxtype::none);
        l->clear_flag(widgetflag::GRAB_MOUSE);
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
        }, {eventid::POINTER_CLICK});

        grid1.add(l, alignmask::CENTER);
    }

    Popup popup(Size(main_screen()->size().w / 2, main_screen()->size().h / 2));
    popup.set_name("popup");
    Button button("Hello World");
    popup.add(button);
    button.set_align(alignmask::CENTER);
    button.set_name("hw");

    ImageButton settings(Image("settings.png"), "", Rect());
    win.add(&settings);
    settings.set_boxtype(Theme::boxtype::none);
    settings.set_align(alignmask::RIGHT | alignmask::TOP, 10);
    settings.on_event([&popup](eventid)
    {
        if (popup.visible())
            popup.hide();
        else
            popup.show(true);
        return 1;
    }, {eventid::POINTER_CLICK});
    win.add(&popup);

    win.add(&player);

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
    }, {eventid::POINTER_CLICK});

    return app.run();
}
