/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/window.h"
#include <algorithm>
#include <vector>
#include <sstream>

using namespace std;

namespace egt
{
    static std::vector<Window*> the_windows;
    static Window* the_main_window = nullptr;
    static auto window_id = 0;

    Window*& main_window()
    {
        return the_main_window;
    }

    std::vector<Window*>& windows()
    {
        return the_windows;
    }

    Window::Window(const Size& size, widgetmask flags)
        : Frame(Rect(size), flags | widgetmask::WINDOW)
    {
        ostringstream ss;
        ss << "window" << window_id++;
        set_name(ss.str());

        DBG("new window " << name() << " " << size);
        windows().push_back(this);

        // if a window size is empty, or this window is the first window, it
        // must be the size of the screen
        if (!the_main_window || size.empty())
        {
            assert(main_screen());

            m_box.x = 0;
            m_box.y = 0;
            m_box.w = main_screen()->size().w;
            m_box.h = main_screen()->size().h;
        }

        if (!the_main_window)
        {
            the_main_window = this;
        }

        // by default, windows are hidden
        hide();

        // go ahead and pick up the default screen
        m_screen = main_screen();
    }

    Window::Window(const Rect& rect, widgetmask flags)
        : Window(rect.size(), flags)
    {
        move(rect.point());
    }

    void Window::set_main_window()
    {
        the_main_window = this;

        assert(main_screen());

        m_box.x = 0;
        m_box.y = 0;
        m_box.w = main_screen()->size().w;
        m_box.h = main_screen()->size().h;

        damage();
    }

    void Window::show()
    {
        for (auto& child : m_children)
        {
            if (child->align() != alignmask::NONE)
            {
                auto r = align_algorithm(child->size(), child->parent()->box(),
                                         child->align(), child->margin());
                child->set_box(r);
            }
        }

        Widget::show();
    }

    Window::~Window()
    {
        auto i = find(the_windows.begin(), the_windows.end(), this);
        if (i != the_windows.end())
        {
            the_windows.erase(i);
        }

        if (the_main_window == this)
        {
            the_main_window = nullptr;

            for (auto& window : the_windows)
            {
                window->set_main_window();
                break;
            }
        }
    }
}
