/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <array>
#include <cmath>
#include <cstdio>
#include <egt/detail/filesystem.h>
#include <egt/detail/screen/kmsscreen.h>
#include <egt/ui>
#include <iostream>
#include <memory>
#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using namespace egt;

//#define DO_SCALING

#ifdef DO_SCALING
/**
 * Calculate a scale relative to how far something is the center.
 */
static float sliding_scale(int win_w, int item_w, int item_pos,
                           float min = 0.5, float max = 1.0)
{
    float range = win_w / 2;
    float delta = std::fabs(range - (item_pos + (item_w / 2)));
    float scale = 1.0 - (delta / range);
    if (scale < min || scale > max)
        return min;
    return scale;
}
#endif

/**
 * Execute a command.
 */
static std::string exec(const char* cmd, bool wait = false)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    if (wait)
    {
        while (!feof(pipe.get()))
        {
            if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
                result += buffer.data();
        }
    }

    return result;
}

#define ITEM_SPACE 250

/*
 * A launcher menu item.
 */
class LauncherItem : public ImageLabel
{
public:
    LauncherItem(int num, const string& name, const string& description,
                 const string& image, const string& exec, int x = 0, int y = 0)
        : ImageLabel(Image(image),
                     name,
                     Rect(Point(x, y), Size()),
                     alignmask::center),
          m_num(num),
          m_name(name),
          m_description(description),
          m_exec(exec)
    {
        set_color(Palette::ColorId::text, Palette::white);
        set_image_align(alignmask::center | alignmask::top);
        set_text_align(alignmask::center | alignmask::bottom);
    }

    void handle(Event& event) override
    {
        ImageLabel::handle(event);

        switch (event.id())
        {
        case eventid::pointer_click:
        {
            main_app().event().quit();

#ifdef HAVE_LIBPLANES
            // TODO: explicitly close KMS
            if (detail::KMSScreen::instance())
                detail::KMSScreen::instance()->close();
#endif

            string cmd = "../share/egt/examples/launcher/launch.sh " + m_exec + " &";
            exec(cmd.c_str());

            event.stop();
            break;
        }
        default:
            break;
        }
    }

    void scale_box(int pos)
    {
        ignoreparam(pos);

#ifdef DO_SCALING
        auto c = center();
        float s = sliding_scale(parent()->w(), w(), pos);
        label_enabled(s > 0.9);
        scale_image(s, true);
        move_to_center(c);
#endif
    }

    inline int num() const { return m_num; }

private:
    int m_num;
    string m_name;
    string m_description;
    string m_exec;
};

/**
 * Main launcher window.
 */
class LauncherWindow : public TopWindow
{
public:
    LauncherWindow()
        : m_popup(main_screen()->size() / 2),
          m_animation(0, 0, std::chrono::milliseconds(200),
                      easing_quintic_easein)
    {
        m_animation.on_change(std::bind(&LauncherWindow::move_boxes, this,
                                        std::placeholders::_1));

        auto hello = make_shared<Button>("Hello World");
        hello->set_align(alignmask::center);
        m_popup.add(hello);

        add(make_shared<ImageLabel>(Image("background.png")));

        auto logo = std::make_shared<ImageLabel>(Image("@microchip_logo_white.png"));
        logo->set_align(alignmask::left | alignmask::top);
        logo->set_margin(10);
        add(logo);

        auto settings = std::make_shared<ImageButton>(Image("settings.png"), "", Rect());
        settings->flags().clear(Widget::flag::grab_mouse);
        add(settings);
        settings->set_boxtype(Theme::boxtype::none);
        settings->set_align(alignmask::right | alignmask::top);
        settings->set_margin(10);
        settings->on_event([this](Event&)
        {
            if (m_popup.visible())
                m_popup.hide();
            else
                m_popup.show(true);
        }, {eventid::pointer_click});
    }

    virtual int load(const std::string& expr)
    {
        std::vector<std::string> files = detail::glob(expr);

        for (auto& file : files)
        {
            rapidxml::file<> xml_file(file.c_str());
            rapidxml::xml_document<> doc;
            doc.parse<0>(xml_file.data());

            auto num = 0;
            auto root_node = doc.first_node("menu");
            for (auto node = root_node->first_node("item"); node; node = node->next_sibling())
            {
                string name = node->first_attribute("name")->value();
                string description = node->first_node("description")->value();
                string image = node->first_node("image")->value();
                string cmd = node->first_node("exec")->value();

                auto box = make_shared<LauncherItem>(num++, name, description, image, cmd);
                add(box);
                box->move_to_center(Point(m_boxes.size() * ITEM_SPACE, h() / 2));

#ifdef DO_SCALING
                // pre-seed the image cache
                for (auto s = 0.5; s <= 2.0; s += 0.01)
                    box->scale_box(s);
#endif

                box->scale_box(m_boxes.size() * ITEM_SPACE - box->w() / 2);

                m_boxes.push_back(box);
            }
        }

        start_snap();

        // super nasty having to put this here
        add(m_popup);

        return 0;
    }

    void handle(Event& event) override
    {
        TopWindow::handle(event);

        switch (event.id())
        {
        case eventid::raw_pointer_down:
            if (!m_moving)
            {
                m_moving = true;
                m_moving_x = event.pointer().point.x;
                m_offset = m_boxes[0]->center().x;
            }
            break;
        case eventid::raw_pointer_up:
            m_moving = false;
            start_snap();
            break;
        case eventid::raw_pointer_move:
            if (m_moving)
            {
                move_boxes(event.pointer().point.x);
                event.stop();
            }
            break;
        default:
            break;
        }
    }

    void move_boxes(int x)
    {
        auto diff = x - m_moving_x;

        for (auto& box : m_boxes)
        {
            auto pos = m_offset + (box->num() * ITEM_SPACE) + diff;

            Rect to(box->box());
            to.x = pos;

            bool visible = Rect::intersect(Rect::merge(to, box->box()), this->box());
            if (visible)
            {
                box->move_to_center(Point(pos, box->center().y));
                box->scale_box(pos - box->w() / 2);
            }
            else
            {
                box->move_to_center(Point(pos, box->center().y));
            }
        }
    }

    void start_snap()
    {
        m_animation.stop();

        auto center = box().center();
        auto distance = w();

        for (auto& box : m_boxes)
        {
            if (center.distance_to<int>(box->box().center()) < std::abs(distance))
            {
                distance = center.distance_to<int>(box->box().center());
                if (center.x < box->box().center().x)
                    distance *= -1;
            }
        }

        m_animation.starting(0);
        m_animation.ending(distance);
        m_animation.duration(std::chrono::milliseconds(static_cast<uint32_t>(std::abs(distance))));
        m_animation.start();

        m_moving_x = 0;
        m_offset = m_boxes[0]->center().x;
    }

private:
    bool m_moving {false};
    int m_moving_x{0};
    int m_offset{0};
    vector<shared_ptr<LauncherItem>> m_boxes;
    Popup m_popup;
    PropertyAnimator m_animation;
};

int main(int argc, const char** argv)
{
    Application app(argc, argv, "launcher");

    LauncherWindow win;
    win.load(detail::exe_pwd() + "/../share/egt/examples/launcher/*.xml");
    win.show();

    return app.run();
}
