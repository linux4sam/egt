/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <cmath>
#include <cstdio>
#include <egt/detail/filesystem.h>
#include <egt/detail/imagecache.h>
#include <egt/ui>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

int main(int argc, const char** argv)
{
    Application app(argc, argv, "gallery");

    std::vector<std::string> files = detail::glob(detail::resolve_file_path("movies/") + "*trailer*.jpg");

    TopWindow win;
    win.set_name("win");
    win.set_color(Palette::ColorId::bg, Palette::black);

    VideoWindow player(Size(320, 192));
    player.set_volume(50);

    auto fullscale = static_cast<double>(main_screen()->size().width) /
                     static_cast<double>(player.width());
    if (fullscale <= 0)
        fullscale = 1.0;

    ImageLabel logo(Image("@128px/microchip_logo_white.png"));
    win.add(logo);
    logo.set_align(alignmask::left | alignmask::top);
    logo.set_margin(10);

    auto grid_height = (win.size().height - logo.height()) / 2;

    ScrolledView view0(Rect(0, logo.height(), win.size().width, grid_height));
    view0.set_color(Palette::ColorId::bg, Palette::black);
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
        l->on_event([&player, file, &animator, &videoshown](Event&)
        {
            cout << "playing " << file << ".avi" << endl;
            player.set_media(file + ".avi");
            player.play();
            player.show();
            videoshown = true;

            animator.clear_change_callbacks();
            animator.on_change(std::bind(&VideoWindow::set_scale, std::ref(player), std::placeholders::_1));
            animator.start();
        }, {eventid::pointer_click});

        l->set_align(alignmask::center);
        grid0.add(l);
    }

    ScrolledView view1(Rect(0, logo.height() + grid_height + 1, win.size().width, grid_height));
    view1.set_color(Palette::ColorId::bg, Palette::black);
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
        l->on_event([&player, file, &animator, &videoshown](Event&)
        {
            cout << "playing " << file << ".avi" << endl;
            player.set_media(file + ".avi");
            player.play();
            player.show();
            videoshown = true;

            animator.clear_change_callbacks();
            animator.on_change(std::bind(&VideoWindow::set_scale, std::ref(player), std::placeholders::_1));
            animator.start();


        }, {eventid::pointer_click});

        l->set_align(alignmask::center);
        grid1.add(l);
    }

    Popup popup(Size(main_screen()->size().width / 2, main_screen()->size().height / 2));
    popup.set_name("popup");
    Button button("Hello World");
    popup.add(button);
    button.set_align(alignmask::center);
    button.set_name("hw");

    ImageButton settings(Image("settings.png"), "", Rect());
    win.add(settings);
    settings.set_boxtype(Theme::boxtype::none);
    settings.set_align(alignmask::right | alignmask::top);
    settings.set_margin(10);
    settings.on_event([&popup](Event&)
    {
        if (popup.visible())
            popup.hide();
        else
            popup.show_centered();
        return 1;
    }, {eventid::pointer_click});
    win.add(popup);

    win.add(player);

    win.show();

    Input::global_input().on_event([&player, &videoshown](Event & event)
    {
        if (videoshown)
        {
            player.pause();
            player.hide();
            videoshown = false;
            event.stop();
        }
    }, {eventid::pointer_click});

    return app.run();
}
