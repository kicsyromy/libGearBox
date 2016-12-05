#ifndef LIBRT_SESSION_P_H
#define LIBRT_SESSION_P_H

#include <vector>

#include <sequential.h>
#include <json.hpp>

#include "librt_http_interface_p.h"
#if defined(PLATFORM_WINDOWS)
#include "librt_http_win_p.h"
#elif defined(PLATFORM_LINUX)
#include "librt_http_linux_p.h"
#elif defined(PLATFORM_MACOS)
#include "librt_http_macos_p.h"
#else
#error "Unsupported platform"
#endif

#include "librt_error.h"

namespace librt
{
    namespace session
    {
        struct Request
        {
            ATTRIBUTE(nlohmann::json, arguments)
            ATTRIBUTE(std::string, method)
            ATTRIBUTE(std::uint16_t, tag)
            INIT_ATTRIBUTES(arguments, method, tag)

            template<typename ...Args>
            Request(const Args &... args) : attributes(args...) {}
            Request() : attributes() {}
            Request &operator =(Request &&other) { attributes = std::move(other.attributes); return *this; }
        };

        struct Response
        {
            ATTRIBUTE(nlohmann::json, arguments)
            ATTRIBUTE(std::string, result)
            ATTRIBUTE(std::uint16_t, tag)
            INIT_ATTRIBUTES(arguments, result, tag)

            Response() : attributes(), error() {}
            Response(Response &&other) :
                attributes(std::move(other.attributes)),
                error(std::move(other.error))
            {}
            Response &operator =(Response &&other)
            {
                attributes = std::move(other.attributes); return *this;
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
            INIT_ATTRIBUTES(activeTorrentCount, downloadSpeed, pausedTorrentCount, torrentCount, uploadSpeed)
        };
    }

#if defined(PLATFORM_WINDOWS)
    using HttpRequestHandler = librt::http::Interface<librt::WinHttp>;
#elif defined(PLATFORM_LINUX)
    using HttpRequestHandler = librt::http::Interface<librt::CUrlHttp>;
#elif defined(PLATFORM_MACOS)
    using HttpRequestHandler = librt::http::Interface<librt::CocoaHttp>;
#else
#error "Unsupported platform"
#endif
    class SessionPrivate
    {
        friend class Session;

    public:
        SessionPrivate(const std::string &url,
                       const std::string &path,
                       std::int32_t port,
                       bool authenticationRequired,
                       const std::string &username,
                       const std::string &password);

        SessionPrivate(std::string &&url,
                       std::string &&path,
                       std::int32_t port,
                       bool authenticationRequired,
                       std::string &&username,
                       std::string &&password);

    public:
        session::Response sendRequest(const std::string &method, nlohmann::json arguments = nlohmann::json());

    private:
        std::string sessionId_;
        HttpRequestHandler http_;
    };
}

#endif // LIBRT_SESSION_P_H
