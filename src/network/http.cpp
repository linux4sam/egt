/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "egt/app.h"
#include "egt/eventloop.h"
#include "egt/network/http.h"
#include <cassert>
#include <chrono>
#include <curl/curl.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

using namespace std;

namespace egt
{
inline namespace v1
{

namespace detail
{
using namespace experimental;

struct HttpClientRequestData
{
    CURL* easy{nullptr};
    asio::ip::tcp::socket* socket{nullptr};
    int last_event{CURL_POLL_NONE};
    HttpClientRequest::buffer_type buffer;
};

class HttpClientRequestManager
{
public:

    static HttpClientRequestManager* Instance()
    {
        static auto p = new HttpClientRequestManager;
        return p;
    }

    static int socket_callback(CURL* easy,
                               curl_socket_t s,
                               int what,
                               void* userp,
                               void* socketp)
    {
        auto i = HttpClientRequestManager::Instance()->m_sockets.find(s);
        assert(i != HttpClientRequestManager::Instance()->m_sockets.end());

        HttpClientRequest* session = i->second;
        session->impl()->last_event = what;

        if (what == CURL_POLL_REMOVE)
        {
            // must return 0
            return 0;
        }

        if (what & CURL_POLL_IN)
        {
            session->impl()->socket->async_wait(asio::ip::tcp::socket::wait_read,
                                                std::bind(HttpClientRequestManager::asio_socket_callback, std::placeholders::_1, easy, s, CURL_POLL_IN, session));
        }

        if (what & CURL_POLL_OUT)
        {
            session->impl()->socket->async_wait(asio::ip::tcp::socket::wait_write,
                                                std::bind(HttpClientRequestManager::asio_socket_callback, std::placeholders::_1, easy, s, CURL_POLL_OUT, session));
        }

        // must return 0
        return 0;
    }

    static int timer_callback(CURLM* multi,
                              long timeout_ms,
                              void* userp)
    {
        asio::steady_timer& timer = HttpClientRequestManager::Instance()->m_timer;

        timer.cancel();
        if (timeout_ms > 0)
        {
            timer.expires_after(std::chrono::milliseconds(timeout_ms));
            timer.async_wait(std::bind(&asio_timer_callback, std::placeholders::_1));
        }
        else if (timeout_ms == 0)
        {
            asio::error_code error;
            asio_timer_callback(error);
        }

        return 0;
    }

    static void asio_timer_callback(const asio::error_code& error)
    {
        if (!error)
        {
            CURLMcode mc = curl_multi_socket_action(HttpClientRequestManager::Instance()->m_multi, CURL_SOCKET_TIMEOUT, 0,
                                                    &(HttpClientRequestManager::Instance()->m_running));
            if (mc != CURLM_OK)
            {
                spdlog::error("curl_multi_socket_action error: ", mc);
            }

            check_multi_info();
        }
    }

    static void asio_socket_callback(const asio::error_code& ec,
                                     CURL* easy,
                                     curl_socket_t s,
                                     int what,
                                     HttpClientRequest* session)
    {
        if (ec)
        {
            what = CURL_CSELECT_ERR;
        }

        HttpClientRequestManager* multi = HttpClientRequestManager::Instance();
        CURLMcode rc = curl_multi_socket_action(multi->m_multi, s, what, &multi->m_running);
        if (rc != CURLM_OK)
        {
            spdlog::error("curl_multi_socket_action: ", int(rc));
        }

        check_multi_info();

        if (multi->m_running <= 0)
        {
            multi->m_timer.cancel();
            return;
        }

        if (!ec && multi->m_sockets.find(s) != multi->m_sockets.end())
        {
            if (!session->impl())
                return;

            if (what == CURL_POLL_IN && (session->impl()->last_event & CURL_POLL_IN))
            {
                session->impl()->socket->async_wait(asio::ip::tcp::socket::wait_read,
                                                    std::bind(HttpClientRequestManager::asio_socket_callback, std::placeholders::_1, easy, s, CURL_POLL_IN, session));
            }

            if (what == CURL_POLL_OUT && (session->impl()->last_event & CURL_POLL_OUT))
            {
                session->impl()->socket->async_wait(asio::ip::tcp::socket::wait_write,
                                                    std::bind(HttpClientRequestManager::asio_socket_callback, std::placeholders::_1, easy, s, CURL_POLL_OUT, session));
            }
        }
    }

    static void check_multi_info()
    {
        CURLMsg* msg = nullptr;

        do
        {
            int msgs_left = 0;
            msg = curl_multi_info_read(HttpClientRequestManager::Instance()->m_multi, &msgs_left);
            if (msg && msg->msg == CURLMSG_DONE)
            {
                CURL* easy = msg->easy_handle;
                HttpClientRequest* s;
                curl_easy_getinfo(easy, CURLINFO_PRIVATE, &s);
                s->finish();
                /// @todo when to delete s?
            }
        } while (msg);
    }

    HttpClientRequestManager(const HttpClientRequestManager&) = delete;
    HttpClientRequestManager(HttpClientRequestManager&&) = delete;
    HttpClientRequestManager& operator=(const HttpClientRequestManager&) = delete;
    HttpClientRequestManager& operator=(HttpClientRequestManager&&) = delete;

    CURLM* m_multi;
    int m_running;
    asio::steady_timer m_timer;
    unordered_map<curl_socket_t, HttpClientRequest*> m_sockets;

private:

    HttpClientRequestManager()
        : m_running(0),
          m_timer(main_app().event().io())
    {
        m_multi = curl_multi_init();
        curl_multi_setopt(m_multi, CURLMOPT_SOCKETFUNCTION, HttpClientRequestManager::socket_callback);
        curl_multi_setopt(m_multi, CURLMOPT_TIMERFUNCTION, HttpClientRequestManager::timer_callback);
    }
};

}

namespace experimental
{

static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    auto written = size * nmemb;
    string str(static_cast<const char*>(ptr), written);
    auto s = static_cast<detail::HttpClientRequestData*>(userdata);
    s->buffer.insert(s->buffer.end(), str.begin(), str.end());
    return written;
}

static curl_socket_t opensocket_callback(void* clientp,
        curlsocktype purpose,
        struct curl_sockaddr* address)
{
    curl_socket_t ret = CURL_SOCKET_BAD;

    HttpClientRequest* s = static_cast<HttpClientRequest*>(clientp);

    if (purpose == CURLSOCKTYPE_IPCXN && address->family == AF_INET)
    {
        s->impl()->socket = new asio::ip::tcp::socket(main_app().event().io());

        asio::error_code ec;
        s->impl()->socket->open(asio::ip::tcp::v4(), ec);
        if (ec)
        {
            throw std::runtime_error("failed to open socket");
        }
        else
        {
            ret = s->impl()->socket->native_handle();
            detail::HttpClientRequestManager::Instance()->m_sockets.insert(std::make_pair(ret, s));
        }
    }
    else
    {
        throw std::runtime_error("unsupported socket type");
    }

    return ret;
}

static int closesocket_callback(void* clientp, curl_socket_t item)
{
    auto s = static_cast<HttpClientRequest*>(clientp);
    assert(s);
    asio::error_code ec;
    if (s->impl()->socket)
        s->impl()->socket->close(ec);
    detail::HttpClientRequestManager::Instance()->m_sockets.erase(item);
    return ec ? ec.value() : 0;
}

HttpClientRequest::HttpClientRequest(const std::string& url)
    : m_url(url),
      m_impl(new detail::HttpClientRequestData)
{}

void HttpClientRequest::start(finish_callback_t finish)
{
    m_finish_callback = finish;
    m_impl->easy = curl_easy_init();

    if (!m_impl->easy)
        throw std::runtime_error("curl_easy_init failed");

    curl_easy_setopt(m_impl->easy, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(m_impl->easy, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(m_impl->easy, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_impl->easy, CURLOPT_WRITEDATA, m_impl.get());
    //curl_easy_setopt(m_impl->easy, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(m_impl->easy, CURLOPT_PRIVATE, this);
    curl_easy_setopt(m_impl->easy, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(m_impl->easy, CURLOPT_LOW_SPEED_TIME, 3L);
    curl_easy_setopt(m_impl->easy, CURLOPT_LOW_SPEED_LIMIT, 10L);
    curl_easy_setopt(m_impl->easy, CURLOPT_OPENSOCKETFUNCTION, opensocket_callback);
    curl_easy_setopt(m_impl->easy, CURLOPT_OPENSOCKETDATA, this);
    curl_easy_setopt(m_impl->easy, CURLOPT_CLOSESOCKETFUNCTION, closesocket_callback);
    curl_easy_setopt(m_impl->easy, CURLOPT_CLOSESOCKETDATA, this);
    curl_multi_add_handle(detail::HttpClientRequestManager::Instance()->m_multi, m_impl->easy);
}

void HttpClientRequest::finish()
{
    m_finish_callback(m_url, std::move(m_impl->buffer));
    cleanup();
}

void HttpClientRequest::cleanup()
{
    if (m_impl->easy)
    {
        curl_multi_remove_handle(detail::HttpClientRequestManager::Instance()->m_multi, m_impl->easy);
        curl_easy_cleanup(m_impl->easy);
    }

    if (m_impl->socket)
    {
        detail::HttpClientRequestManager::Instance()->m_sockets.erase(m_impl->socket->native_handle());
        delete m_impl->socket;
    }
}

HttpClientRequest::~HttpClientRequest()
{
    cleanup();
}

}
}
}
