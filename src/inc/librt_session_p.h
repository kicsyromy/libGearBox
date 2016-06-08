#ifndef LIBRT_SESSION_P_H
#define LIBRT_SESSION_P_H

#include <vector>

#include <sequential.h>
#include <json.hpp>

#include "librt_torrent.h"

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

            Response() : attributes() {}
            Response(Response &&other) : attributes(std::move(other.attributes)) {}
            Response &operator =(Response &&other) { attributes = std::move(other.attributes); return *this; }
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

    class SessionPrivate
    {
        friend class Session;

    public:
        SessionPrivate(const std::string &url,
                       const std::string &path,
                       bool authenticationRequired,
                       const std::string &username,
                       const std::string &password);

        SessionPrivate(std::string &&url,
                       std::string &&path,
                       bool authenticationRequired,
                       std::string &&username,
                       std::string &&password);

    public:
        session::Response sendRequest(const std::string &method, nlohmann::json arguments = nlohmann::json(),
                                      bool *error = nullptr, std::string *errorString = nullptr);

    private:
        std::string url_;
        std::string path_;
        bool authenticationRequired_;
        std::string username_;
        std::string password_;
    };
}

#endif // LIBRT_SESSION_P_H
