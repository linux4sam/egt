/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_NETWORK_HTTP_H
#define EGT_NETWORK_HTTP_H

/**
 * @file
 * @brief Working with http.
 */

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
 * request.start_async(url, [](const unsigned char* data, size_t len, bool done){
 *     ...
 * });
 * @endcode
 */
class EGT_API HttpClientRequest : public detail::NonCopyable
{
public:

    using ReadCallback = std::function < void(const unsigned char* data, size_t len, bool done) >;

    /**
     * Create a request for the specified URL.
     */
    explicit HttpClientRequest();

    /**
     * Start the download.
     */
    virtual void start_async(const std::string& url, ReadCallback callback);

    /// @private
    inline detail::HttpClientRequestData* impl()
    {
        return m_impl.get();
    }

    virtual ~HttpClientRequest();

protected:

    void finish();
    void cleanup();

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
