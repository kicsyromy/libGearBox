#include "librt_session.h"

#include <string>

#include <formats/json_format.h>
#include <cpr/cpr.h>

#include "librt_session_p.h"
#include "librt_torrent_p.h"

#include "librt_logger_p.h"

using nlohmann::json;
using namespace librt;

namespace
{
    constexpr std::uint16_t SESSION_TAG = 33872;
    constexpr std::uint8_t  RETRY_COUNT = 5;

    std::string sessionId;

    std::string getSessionId(const std::string &text)
    {
        static constexpr std::uint8_t SESSION_ID_OFFSET = 27;
        static constexpr std::uint8_t SESSION_ID_LEN    = 48;

        std::string sessionId;

        auto position = text.rfind("X-Transmission-Session-Id");
        if (position != std::string::npos)
        {
            sessionId = text.substr(position + SESSION_ID_OFFSET, SESSION_ID_LEN);
        }

        return std::move(sessionId);
    }
}

session::Response session::sendRequest(nlohmann::json arguments, const std::string &method, bool *error, std::string *errorString)
{
    session::Request  request(arguments, method, SESSION_TAG);
    session::Response response;
    JsonFormat jsonFormat;
    sequential::to_format(jsonFormat, request);

    if (error != nullptr)
        *error = true;
    if (errorString != nullptr)
        *errorString = "Unknown Error";

    LOG_DEBUG("Requesting: \n{}", jsonFormat.output().dump(4));

    /* As per the Transmission documentation:                                   */
    /* Most Transmission RPC servers require a X-Transmission-Session-Id        */
    /* header to be sent with requests, to prevent CSRF attacks.                */
    /* When your request has the wrong id -- such as when you send your first   */
    /* request, or when the server expires the CSRF token -- the                */
    /* Transmission RPC server will return an HTTP 409 error with the           */
    /* right X-Transmission-Session-Id in its own headers.                      */
    /* So, the correct way to handle a 409 response is to update your           */
    /* X-Transmission-Session-Id and to resend the previous request.            */
    for (std::uint8_t it = 0; it < RETRY_COUNT; ++it)
    {
        if (result.status_code == 409)
        {
            sessionId = getSessionId(result.text);
        }
        else
        {
            jsonFormat.parse(result.text);
            sequential::from_format(jsonFormat, response);
            if (error != nullptr)
                *error = false;
            break;
        }
    }

    const auto &result = response.get_result();
    if (result != "success" && !result.empty())
    {
        if (error != nullptr)
            *error = true;
        if (errorString != nullptr)
            *errorString = result;
    }

    return std::move(response);
}

std::vector<Torrent> librt::session::getTorrents()
{
    std::vector<Torrent> retValue;
    std::vector<TorrentPrivate> torrents;
    TorrentPrivate::Response torrentResponse;

    const auto &fields = TorrentPrivate::attribute_names();
    nlohmann::json requestValues;
    requestValues["fields"] = fields;
    session::Response response(std::move(sendRequest(requestValues, "torrent-get")));

    LOG_DEBUG("Result: {}", response.get_result());

    JsonFormat jsonFormat;
    jsonFormat.fromJson(response.get_arguments());
    sequential::from_format(jsonFormat, torrentResponse);
    torrents = torrentResponse.get_torrents();
    for (TorrentPrivate &torrentPriv: torrents)
    {
        retValue.emplace_back(new TorrentPrivate(std::move(torrentPriv)));
    }

    return std::move(retValue);
}
