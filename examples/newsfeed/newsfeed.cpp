/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <egt/detail/filesystem.h>
#include <egt/detail/imagecache.h>
#include <egt/detail/string.h>
#include <egt/network/http.h>
#include <egt/ui>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <sstream>
#include <string>

using namespace std;
using namespace egt;
using namespace egt::experimental;
using namespace egt::detail;

using download_callback = std::function<void(const HttpClientRequest::buffer_type& data)>;

static void download(const std::string& url, download_callback callback)
{
    cout << "request " << url << endl;

    auto filename = detail::extract_filename(url);
    if (!filename.empty() && detail::exists(filename))
    {
        callback(detail::read_file(filename));
        return;
    }

    // TODO: memory leak
    auto session = new HttpClientRequest(url);
    session->start([filename, callback](const std::string & url, HttpClientRequest::buffer_type && html)
    {

        if (!filename.empty())
        {
            std::ofstream out(filename, std::ofstream::binary |
                              std::ofstream::trunc | std::ofstream::out);
            out.write(html.data(), html.size());
            out.close();
        }

        callback(html);
    });
}

class NewsItem : public VerticalBoxSizer
{
public:

    NewsItem(const std::string& title, const std::string& desc,
             const std::string& date, const std::string& link,
             const std::string& image)
        : VerticalBoxSizer(justification::start),
          m_line(*this),
          m_title(*this, truncate(title, 80)),
          m_date(*this, date),
          m_desc(*this, truncate(desc, 80)),
          m_link(*this, truncate(link, 80))
    {
        m_title.set_font(Font(20, Font::weightid::bold));
        m_date.set_font(Font(Font::slantid::oblique));
        m_link.set_font(Font(10));
        m_link.set_color(Palette::ColorId::label_text, Palette::blue);

        set_align(alignmask::expand_horizontal);

        if (!image.empty())
        {
            download(image, [this, image](const HttpClientRequest::buffer_type & data)
            {
                // this is silly, we have the data passed as data but still load from file

                auto filename = detail::extract_filename(image);
                if (!filename.empty())
                {
                    auto surface = detail::image_cache().get(filename);
                    if (surface)
                    {
                        assert(!m_image);
                        m_image = make_shared<ImageLabel>(surface);
                        this->add(m_image);
                    }
                }
            });
        }

        m_line.set_padding(10);
        expand_horizontal(m_line);
        //expand_horizontal(m_title);
        //expand_horizontal(m_date);
        //expand_horizontal(m_desc);
        //expand_horizontal(m_link);
    }

    virtual ~NewsItem() = default;

protected:

    NewsItem() = delete;
    LineWidget m_line;
    Label m_title;
    Label m_date;
    Label m_desc;
    Label m_link;
    shared_ptr<ImageLabel> m_image;
};

template<class T>
static int load(const string& file, T& list)
{
    list.remove_all();

    auto total = 10;

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
        string date;
        string image;

        auto t = node->first_node("title");
        if (t)
            title = t->first_node()->value();

        auto d = node->first_node("description");
        if (d)
        {
            description = d->first_node()->value();
            if (description == "null")
                description.clear();
        }

        auto p = node->first_node("pubDate");
        if (p)
            date = p->first_node()->value();

        auto l = node->first_node("link");
        if (l)
            link = l->first_node()->value();

        auto i = node->first_node("image");
        if (i)
            image = i->first_node()->value();

        auto item = make_shared<NewsItem>(title, description, date, link, image);
        list.add(item);

        if (--total == 0)
            break;
    }

    return 0;
}

int main(int argc, const char** argv)
{
    string url = "https://www.microchip.com/RSS/Recent-AppNotes.xml";
    if (argc >= 2)
        url = argv[1];

    Application app(argc, argv, "newsfeed");

    TopWindow win;

    ScrolledView view(win, ScrolledView::policy::never);
    expand(view);

    VerticalBoxSizer list(view);
    expand_horizontal(list);

    auto item = make_shared<Label>("Loading " + url);
    list.add(item);

    win.show();

    download(url, [&list](const HttpClientRequest::buffer_type & data)
    {
        if (!data.empty())
        {
            std::ofstream out("dynfeed.xml");
            out.write(data.data(), data.size());
            out.close();
            load("dynfeed.xml", list);
        }
    });

    return app.run();
}
