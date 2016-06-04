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

        Response sendRequest(nlohmann::json arguments, const std::string &method,
                             bool *error = nullptr, std::string *errorString = nullptr);
    }
}

#endif // LIBRT_SESSION_P_H
