#ifndef LIBGEARBOX_SESSION_P_H
#define LIBGEARBOX_SESSION_P_H

#include <vector>

#include <sequential.h>
#include <json.hpp>

#include "libgearbox_http_interface_p.h"
#if defined(PLATFORM_WINDOWS)
#include "libgearbox_http_win_p.h"
#elif defined(PLATFORM_LINUX)
#include "libgearbox_http_linux_p.h"
#elif defined(PLATFORM_MACOS)
#include "libgearbox_http_macos_p.h"
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
    using HttpRequestHandler = gearbox::http::Interface<gearbox::WinHttp>;
#elif defined(PLATFORM_LINUX)
    using HttpRequestHandler = gearbox::http::Interface<gearbox::CUrlHttp>;
#elif defined(PLATFORM_MACOS)
    using HttpRequestHandler = gearbox::http::Interface<gearbox::CocoaHttp>;
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

#endif // LIBGEARBOX_SESSION_P_H
