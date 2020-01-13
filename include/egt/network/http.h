/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_NETWORK_HTTP_H
#define EGT_NETWORK_HTTP_H

#include <egt/detail/meta.h>
#include <memory>
#include <string>
#include <vector>

namespace egt
{
inline namespace v1
{

namespace detail
{
struct HttpClientRequestData;
class HttpClientRequestManager;
}

namespace experimental
{

/**
 * An HTTP client request.
 *
 * This works as an asynchronous HTTP request handler that uses the EGT event
 * loop to process the request.
 *
 * @code{.cpp}
 * HttpClientRequest request("http://example.com");
 * request.start([](const std::string& url, buffer_type && buffer){
 *     ...
 * });
 * @endcode
 */
class EGT_API HttpClientRequest : public detail::NonCopyable
{
public:

    HttpClientRequest() = delete;

    using buffer_type = std::vector<char>;

    using FinishCallback = std::function < void(const std::string& url, buffer_type && buffer) >;

    /**
     * Create a request for the specified URL.
     */
    explicit HttpClientRequest(const std::string& url);

    /**
     * Start the download.
     */
    virtual void start(FinishCallback finish);

    /**
     * @private
     */
    inline detail::HttpClientRequestData* impl()
    {
        return m_impl.get();
    }

    virtual ~HttpClientRequest();

protected:

    void finish();
    void cleanup();

    /**
     * Requested URL.
     */
    std::string m_url;

    /**
     * Finish callback.
     */
    FinishCallback m_finish_callback;

    /**
     * Implementation pointer.
     */
    std::unique_ptr<detail::HttpClientRequestData> m_impl;

    friend class detail::HttpClientRequestManager;
};

}
}
}

#endif
