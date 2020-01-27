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

namespace egt
{
inline namespace v1
{

namespace detail
{
using namespace experimental;

struct HttpClientRequestData
{
    std::string url;
    CURL* easy{nullptr};
    std::unique_ptr<asio::ip::tcp::socket> socket;
    int last_event{CURL_POLL_NONE};
    HttpClientRequest::ReadCallback m_read_callback;

    inline void on_read(const unsigned char* data, size_t len, bool done = false)
    {
        SPDLOG_TRACE("http read data len {}", len);

        if (m_read_callback)
            m_read_callback(data, len, done);
    }
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
        if (i != HttpClientRequestManager::Instance()->m_sockets.end())
        {
            auto request = i->second;
            request->impl()->last_event = what;

            if (what == CURL_POLL_REMOVE)
            {
                // must return 0
                return 0;
            }

            if (what & CURL_POLL_IN)
            {
                request->impl()->socket->async_wait(asio::ip::tcp::socket::wait_read,
                                                    std::bind(HttpClientRequestManager::asio_socket_callback, std::placeholders::_1, easy, s, CURL_POLL_IN, request));
            }

            if (what & CURL_POLL_OUT)
            {
                request->impl()->socket->async_wait(asio::ip::tcp::socket::wait_write,
                                                    std::bind(HttpClientRequestManager::asio_socket_callback, std::placeholders::_1, easy, s, CURL_POLL_OUT, request));
            }
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
                spdlog::warn("curl_multi_socket_action error: {}", mc);
            }

            check_multi_info();
        }
    }

    static void asio_socket_callback(const asio::error_code& ec,
                                     CURL* easy,
                                     curl_socket_t s,
                                     int what,
                                     HttpClientRequest* request)
    {
        if (ec)
        {
            what = CURL_CSELECT_ERR;
        }

        HttpClientRequestManager* multi = HttpClientRequestManager::Instance();
        CURLMcode rc = curl_multi_socket_action(multi->m_multi, s, what, &multi->m_running);
        if (rc != CURLM_OK)
        {
            spdlog::warn("curl_multi_socket_action: {}", int(rc));
        }

        check_multi_info();

        if (multi->m_running <= 0)
        {
            multi->m_timer.cancel();
            return;
        }

        if (!ec && multi->m_sockets.find(s) != multi->m_sockets.end())
        {
            if (!request->impl())
                return;

            if (what == CURL_POLL_IN && (request->impl()->last_event & CURL_POLL_IN))
            {
                request->impl()->socket->async_wait(asio::ip::tcp::socket::wait_read,
                                                    std::bind(HttpClientRequestManager::asio_socket_callback, std::placeholders::_1, easy, s, CURL_POLL_IN, request));
            }

            if (what == CURL_POLL_OUT && (request->impl()->last_event & CURL_POLL_OUT))
            {
                request->impl()->socket->async_wait(asio::ip::tcp::socket::wait_write,
                                                    std::bind(HttpClientRequestManager::asio_socket_callback, std::placeholders::_1, easy, s, CURL_POLL_OUT, request));
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
                if (easy)
                {
                    HttpClientRequest* s{nullptr};
                    curl_easy_getinfo(easy, CURLINFO_PRIVATE, &s);
                    if (s)
                    {
                        s->finish();
                    }
                }
            }
        } while (msg);
    }

    HttpClientRequestManager(const HttpClientRequestManager&) = delete;
    HttpClientRequestManager(HttpClientRequestManager&&) = delete;
    HttpClientRequestManager& operator=(const HttpClientRequestManager&) = delete;
    HttpClientRequestManager& operator=(HttpClientRequestManager&&) = delete;

    CURLM* m_multi{};
    int m_running{0};
    asio::steady_timer m_timer;
    std::unordered_map<curl_socket_t, HttpClientRequest*> m_sockets;

private:

    HttpClientRequestManager()
        : m_multi(curl_multi_init()),
          m_timer(Application::instance().event().io())
    {
        curl_multi_setopt(m_multi, CURLMOPT_SOCKETFUNCTION, HttpClientRequestManager::socket_callback);
        curl_multi_setopt(m_multi, CURLMOPT_TIMERFUNCTION, HttpClientRequestManager::timer_callback);
    }
};

}

namespace experimental
{

static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    const auto written = size * nmemb;
    auto s = static_cast<detail::HttpClientRequestData*>(userdata);
    if (s)
        s->on_read(reinterpret_cast<const unsigned char*>(ptr), written, false);
    return written;
}

static curl_socket_t opensocket_callback(void* clientp,
        curlsocktype purpose,
        struct curl_sockaddr* address)
{
    curl_socket_t ret = CURL_SOCKET_BAD;

    HttpClientRequest* s = static_cast<HttpClientRequest*>(clientp);
    if (s)
    {
        if (purpose == CURLSOCKTYPE_IPCXN && address->family == AF_INET)
        {
            s->impl()->socket = detail::make_unique<asio::ip::tcp::socket>(Application::instance().event().io());

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
    }

    return ret;
}

static int closesocket_callback(void* clientp, curl_socket_t item)
{
    auto s = static_cast<HttpClientRequest*>(clientp);
    if (s)
    {
        asio::error_code ec;
        if (s->impl()->socket)
            s->impl()->socket->close(ec);
        detail::HttpClientRequestManager::Instance()->m_sockets.erase(item);
        return ec ? ec.value() : 0;
    }

    return 0;
}

HttpClientRequest::HttpClientRequest()
    : m_impl(std::make_unique<detail::HttpClientRequestData>())
{}

void HttpClientRequest::start_async(const std::string& url, ReadCallback callback)
{
    cleanup();

    m_impl->url = url;
    m_impl->m_read_callback = callback;
    m_impl->easy = curl_easy_init();

    if (!m_impl->easy)
        throw std::runtime_error("curl_easy_init failed");

    curl_easy_setopt(m_impl->easy, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(m_impl->easy, CURLOPT_URL, m_impl->url.c_str());
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
    m_impl->on_read(nullptr, 0, true);
    cleanup();
}

// Not safe to call this in any curl callback.
//
// curl_multi_remove_handle:
//    It is fine to remove a handle at any time during a
//    transfer, just not from within any libcurl callback
//    function.
void HttpClientRequest::cleanup()
{
    if (m_impl->socket)
    {
        detail::HttpClientRequestManager::Instance()->m_sockets.erase(m_impl->socket->native_handle());
        m_impl->socket.reset();
    }

    if (m_impl->easy)
    {
        curl_multi_remove_handle(detail::HttpClientRequestManager::Instance()->m_multi, m_impl->easy);
        curl_easy_cleanup(m_impl->easy);
        m_impl->easy = nullptr;
    }
}

HttpClientRequest::~HttpClientRequest()
{
    cleanup();
}

}
}
}
