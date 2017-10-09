/*
 * Copyright (c) 2016 Romeo Calota
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Author: Romeo Calota
 */

#ifndef LIBGEARBOX_SESSION_P_H
#define LIBGEARBOX_SESSION_P_H

#include <vector>

#include <json.hpp>
#include <sequential.h>

#include "libgearbox_http_interface_p.h"
#if defined(PLATFORM_WINDOWS)
#include "libgearbox_http_win_p.h"
using HttpRequestHandler = gearbox::http::Interface<gearbox::WinHttp>;
#elif defined(PLATFORM_LINUX)
#include "libgearbox_http_linux_p.h"
using HttpRequestHandler = gearbox::http::Interface<gearbox::CUrlHttp>;
#elif defined(PLATFORM_MACOS)
#include "libgearbox_http_macos_p.h"
using HttpRequestHandler = gearbox::http::Interface<gearbox::CocoaHttp>;
#elif defined(PLATFORM_UWP)
#include "libgearbox_http_uwp_p.h"
using HttpRequestHandler = gearbox::http::Interface<gearbox::HttpUWP>;
#else
#error "Unsupported platform"
#endif

#include "libgearbox_error.h"

namespace gearbox
{
    namespace session
    {
        struct Request
        {
            ATTRIBUTE(nlohmann::json, arguments)
            ATTRIBUTE(std::string, method)
            ATTRIBUTE(std::uint16_t, tag)
            INIT_ATTRIBUTES(arguments, method, tag)

            template <typename... Args>
            Request(const Args &... args) : attributes(args...)
            {
            }
            Request() : attributes() {}
            Request &operator=(Request &&other)
            {
                attributes = std::move(other.attributes);
                return *this;
            }
        };

        struct Response
        {
            ATTRIBUTE(nlohmann::json, arguments)
            ATTRIBUTE(std::string, result)
            ATTRIBUTE(std::uint16_t, tag)
            INIT_ATTRIBUTES(arguments, result, tag)

            Response() : attributes(), error() {}
            Response(Response &&other)
              : attributes(std::move(other.attributes)),
                error(std::move(other.error))
            {
            }
            Response &operator=(Response &&other)
            {
                attributes = std::move(other.attributes);
                return *this;
                error = std::move(other.error);
            }

            Error error;
        };

        struct Statistics
        {
            ATTRIBUTE(std::int32_t, activeTorrentCount)
            ATTRIBUTE(std::int32_t, downloadSpeed)
            ATTRIBUTE(std::int32_t, pausedTorrentCount)
            ATTRIBUTE(std::int32_t, torrentCount)
            ATTRIBUTE(std::int32_t, uploadSpeed)
            INIT_ATTRIBUTES(activeTorrentCount,
                            downloadSpeed,
                            pausedTorrentCount,
                            torrentCount,
                            uploadSpeed)
        };
    }

    class SessionPrivate
    {
        friend class Session;

    public:
        SessionPrivate(const std::string &host,
                       const std::string &path,
                       std::int32_t port,
                       bool authenticationRequired,
                       const std::string &username,
                       const std::string &password);

        SessionPrivate(std::string &&host,
                       std::string &&path,
                       std::int32_t port,
                       bool authenticationRequired,
                       std::string &&username,
                       std::string &&password);

    public:
        session::Response sendRequest(
            const std::string &method,
            nlohmann::json arguments = nlohmann::json());

    private:
        std::string sessionId_;
        HttpRequestHandler http_;
    };
}

#endif // LIBGEARBOX_SESSION_P_H
