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

#ifdef HAVE_RAPIDXML_RAPIDXML_HPP
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#else
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#endif

using namespace std;
using namespace mui;

// pull feed from http://feeds.reuters.com/reuters/technologyNews

#if 0
class NewsItem : public ListBoxItem
{
public:

    StringItem(const std::string& title, const std::string& description)
        : m_title(title)
          m_description(description)
    {}

    virtual void draw(Painter& painter, const Rect& rect, bool selected) override
    {
        ListBoxItem::draw(painter, rect, selected);

        m_title.draw();
        m_description.draw();

        painter.set_color(global_palette().color(Palette::TEXT));
        painter.set_font(m_font);
        painter.draw_text(rect, m_text, alignmask::CENTER);
    }

    /**
     * Set the font of the items.
     */
    virtual void font(const Font& font) { m_font = font; }

    virtual ~StringItem()
    {}

protected:

    StringItem() = delete;

    std::string m_title;
    std::string m_description;
    Font m_font;
};
#endif

static int load(const string& file, ListBox& list)
{
    rapidxml::file<> xml_file(file.c_str());
    rapidxml::xml_document<> doc;
    doc.parse<0>(xml_file.data());

    auto rss = doc.first_node("rss");
    auto channel = rss->first_node("channel");
    for (auto node = channel->first_node("item"); node; node = node->next_sibling())
    {
        string title;
        string description;
        string link;

        auto t = node->first_node("title");
        if (t)
        {
            title = t->first_node()->value();
        }

        auto d = node->first_node("description");
        if (d)
        {
            description = d->first_node()->value();
        }

        auto l = node->first_node("description");
        if (l)
        {
            link = l->first_node()->value();
        }

        list.add_item(new StringItem(title));
    }

    return 0;
}

#define SHARED_PATH "../share/mui/examples/newsfeed/"

int main()
{
    Application app;

    Window win;

    ListBox list(win, Rect(Point(), Size(win.w(), win.h())));

    load(SHARED_PATH "feed.xml", list);

    win.show();

    return app.run();
}
