/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 * Joshua Henderson <joshua.henderson@microchip.com>
 */
#include "egt/window.h"
#include <algorithm>
#include <vector>

using namespace std;

namespace egt
{
    static std::vector<Window*> the_windows;
    static Window* the_window = nullptr;
    static auto window_id = 0;

    Window*& main_window()
    {
        return the_window;
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

        DBG("new window " << size);
        windows().push_back(this);

        // if a window size is empty, or this window is the first window, it
        // must be the size of the screen
        if (!the_window || size.empty())
        {
            assert(main_screen());

            m_box.x = 0;
            m_box.y = 0;
            m_box.w = main_screen()->size().w;
            m_box.h = main_screen()->size().h;
        }

        if (!the_window)
        {
            the_window = this;
        }

        hide();
        //damage();

        // go ahead and pick up the default screen
        m_screen = main_screen();
    }

    Window::Window(const Rect& rect, widgetmask flags)
        : Window(rect.size(), flags)
    {
        move(rect.point());
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

}
