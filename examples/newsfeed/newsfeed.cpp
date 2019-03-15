/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cmath>
#include <cstdio>
#include <glob.h>
#include <iostream>
#include <map>
#include <memory>
#include <egt/ui>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <string>
#include <vector>
#define USE_LIBCURL
#ifdef USE_LIBCURL
#include <curl/curl.h>
#endif

#ifdef HAVE_RAPIDXML_RAPIDXML_HPP
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#else
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#endif

using namespace std;
using namespace egt;

class NewsItem : public StaticGrid
{
public:

    NewsItem(const std::string& title, const std::string& desc,
             const std::string& date, const std::string& link)
        : StaticGrid(Tuple(1, 2)),
          m_title(title),
          m_desc(desc),
          m_date(date),
          m_link(link)
    {
        add(expand(m_title), 0, 0);
        add(expand(m_date), 0, 1);
    }

    virtual ~NewsItem()
    {}

protected:

    NewsItem() = delete;

    Label m_title;
    Label m_desc;
    Label m_date;
    Label m_link;
};

#ifdef USE_LIBCURL
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static string download(const std::string& url)
{
    string readBuffer;
    CURL* curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    return readBuffer;
}
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
        string date;

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

        auto p = node->first_node("pubDate");
        if (p)
        {
            date = p->first_node()->value();
        }

        auto l = node->first_node("link");
        if (l)
        {
            link = l->first_node()->value();
        }

        list.add_item(make_shared<NewsItem>(title, description, date, link));
    }

    return 0;
}

int main(int argc, const char** argv)
{
    Application app(argc, argv, "newsfeed");

    TopWindow win;

    ListBox list(win);
    list.set_align(alignmask::expand);

#ifdef USE_LIBCURL
    string data = download("http://feeds.reuters.com/reuters/technologyNews");
    if (!data.empty())
    {
        std::ofstream out("dynfeed.xml");
        out << data;
        out.close();
        load("dynfeed.xml", list);
    }
#else
    load(detail::exe_pwd() + "/../share/egt/examples/newsfeed/feed.xml", list);
#endif

    win.show();

    return app.run();
}
