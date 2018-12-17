/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <array>
#include <cmath>
#include <cstdio>
#include <egt/ui>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#ifdef HAVE_RAPIDXML_RAPIDXML_HPP
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#else
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#endif

using namespace std;
using namespace egt;

#define DO_SCALING

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
        : ImageLabel(Image(image), name, Point(x, y)),
          m_num(num),
          m_name(name),
          m_description(description),
          m_exec(exec)
    {
        resize(size() + Size(0, 50));
        palette().set(Palette::TEXT, Palette::GROUP_NORMAL, Color::WHITE);
        Font newfont(24, Font::weightid::BOLD);
        set_font(newfont);
        set_text_align(alignmask::CENTER | alignmask::BOTTOM);
    }

    int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::MOUSE_UP:
        {
            main_app().event().quit();

#ifdef HAVE_LIBPLANES
            // TODO: explicitly close KMS
            if (KMSScreen::instance())
                KMSScreen::instance()->close();
#endif

            string cmd = "../share/egt/examples/launcher/launch.sh " + m_exec + " &";
            exec(cmd.c_str());

            return 1;
        }
        default:
            break;
        }

        return ImageLabel::handle(event);
    }

    void scale_box(int pos)
    {
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

        m_popup.add(new Button("Hello World"))->set_align(alignmask::CENTER);

        add(new ImageLabel(Image("background.png")));

        auto logo = new ImageLabel(Image("@microchip_logo_white.png"));
        add(logo)->set_align(alignmask::LEFT | alignmask::TOP, 10);

        auto settings = new ImageButton(Image("settings.png"), "", Rect());
        settings->flag_clear(widgetmask::GRAB_MOUSE);
        add(settings);
        settings->set_align(alignmask::RIGHT | alignmask::TOP, 10);
        settings->on_event([this](eventid event)
        {
            if (event == eventid::MOUSE_DOWN)
            {
                if (m_popup.visible())
                    m_popup.hide();
                else
                    m_popup.show(true);
            }
            return 0;
        });
    }

    virtual int load(const std::string& expr)
    {
        std::vector<std::string> files = experimental::glob(expr);

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
                string exec = node->first_node("exec")->value();

                LauncherItem* box = new LauncherItem(num++, name, description, image, exec);
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
        add(&m_popup);

        return 0;
    }

    int handle(eventid event) override
    {
        switch (event)
        {
        case eventid::MOUSE_DOWN:
            if (!m_moving)
            {
                m_moving = true;
                m_moving_x = event_mouse().x;
                m_offset = m_boxes[0]->center().x;
            }
            break;
        case eventid::MOUSE_UP:
            m_moving = false;
            start_snap();
            break;
        case eventid::MOUSE_MOVE:
            if (m_moving)
            {
                move_boxes(event_mouse().x);
                return 1;
            }
            break;
        default:
            break;
        }

        return TopWindow::handle(event);
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
        m_animation.duration(std::chrono::milliseconds(std::abs(distance)));
        m_animation.start();

        m_moving_x = 0;
        m_offset = m_boxes[0]->center().x;
    }

private:
    bool m_moving {false};
    int m_moving_x{0};
    int m_offset{0};
    vector<LauncherItem*> m_boxes;
    Popup<Window> m_popup;
    PropertyAnimator m_animation;
};

#define SHARED_PATH "../share/egt/examples/launcher/"

int main(int argc, const char** argv)
{
    Application app(argc, argv);

    set_image_path(SHARED_PATH);

    LauncherWindow win;
    win.load(SHARED_PATH "*.xml");
    win.show();

    return app.run();
}
