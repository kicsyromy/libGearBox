#include "librt_session.h"

#include <string>

#include <formats/json_format.h>
#include <cpr/cpr.h>
#include <fmt/format.h>

#include "librt_session_p.h"
#include "librt_torrent_p.h"

#define DEBUG_BUILD
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

SessionPrivate::SessionPrivate(const std::string &url,
                               const std::string &path,
                               bool authenticationRequired,
                               const std::string &username,
                               const std::string &password) :
    url_(url),
    path_(path),
    authenticationRequired_(authenticationRequired),
    username_(username),
    password_(password)
{
}

SessionPrivate::SessionPrivate(std::string &&url,
                               std::string &&path,
                               bool authenticationRequired,
                               std::string &&username,
                               std::string &&password) :
    url_(url),
    path_(path),
    authenticationRequired_(authenticationRequired),
    username_(username),
    password_(password)
{
}

session::Response SessionPrivate::sendRequest(const std::string &method, nlohmann::json arguments, bool *error, std::string *errorString)
{
    session::Request  request(arguments, method, SESSION_TAG);
    session::Response response;
    JsonFormat jsonFormat;
    sequential::to_format(jsonFormat, request);

    bool isError = false;
    std::string errorStr("Unknown Error");

    LOG_DEBUG("Requesting \"{}\": \n{}", method, jsonFormat.output().dump(4));

    /* As per the Transmission documentation:                                   */
    /* Most Transmission RPC servers require a X-Transmission-Session-Id        */
    /* header to be sent with requests, to prevent CSRF attacks.                */
    /* When your request has the wrong id -- such as when you send your first   */
    /* request, or when the server expires the CSRF token -- the                */
    /* Transmission RPC server will return an HTTP 409 error with the           */
    /* right X-Transmission-Session-Id in its own headers.                      */
    /* So, the correct way to handle a 409 response is to update your           */
    /* X-Transmission-Session-Id and to resend the previous request.            */
    bool finished = false;
    for (std::uint8_t it = 0; (it < RETRY_COUNT) && (!isError) && (!finished); ++it)
    {
        cpr::Response result;
        if (authenticationRequired_)
        {
            result = cpr::Post(fmt::format("{}{}", url_, path_),
                               cpr::Authentication{username_, password_},
                               cpr::Header{{"X-Transmission-Session-Id", sessionId}},
                               cpr::Body(jsonFormat.output().dump()));
        }
        else
        {
            result = cpr::Post(fmt::format("{}{}", url_, path_),
                               cpr::Header{{"X-Transmission-Session-Id", sessionId}},
                               cpr::Body(jsonFormat.output().dump()));
        }

        switch (result.status_code)
        {
        case 409:
            {
                sessionId = getSessionId(result.text);
                break;
            }
        case 405:
            {
                isError = true;
                errorStr = "Method not allowed";
                LOG_DEBUG("Error in request: 405: Method not allowed");
                break;
            }
        case 200:
            {
                jsonFormat.parse(result.text);
                sequential::from_format(jsonFormat, response);
                finished = true;

                const auto &result = response.get_result();
                if (result != "success" && !result.empty())
                {
                    LOG_DEBUG("Error in request: {}", result);
                    isError = true;
                    errorStr = std::move(result);
                }

                break;
            }
        default:
            {
                isError = true;
                break;
            }
        }
    }

    if (error != nullptr)
        *error = isError;
    if (errorString != nullptr)
        *errorString = std::move(errorStr);

    return std::move(response);
}

Session::Session() :
    priv_(new SessionPrivate("", DEFAULT_PATH, false, "", ""))
{
}

Session::Session(const std::string &url,
                 const std::string &path,
                 Authentication authentication,
                 const std::string &username,
                 const std::string &password) :
    priv_(new SessionPrivate(url, path, authentication ==  Authentication::Required, username, password))
{
}

Session::Session(std::string &&url,
                 std::string &&path,
                 Authentication authentication,
                 std::string &&username,
                 std::string &&password) :
    priv_(new SessionPrivate(url, path, authentication ==  Authentication::Required, username, password))
{
}

Session::Statistics Session::statistics() const
{
    session::Response response(std::move(priv_->sendRequest("session-stats")));
    session::Statistics stats;
    JsonFormat jsonFormat(response.get_arguments());
    sequential::from_format(jsonFormat, stats);

    return {
        stats.get_torrentCount(),
        stats.get_activeTorrentCount(),
        stats.get_pausedTorrentCount(),
        stats.get_downloadSpeed(),
        stats.get_uploadSpeed()
    };
}

std::vector<Torrent> Session::torrents() const
{
    std::vector<Torrent> retValue;
    std::vector<TorrentPrivate> torrents;
    TorrentPrivate::Response torrentResponse;

    const auto &fields = TorrentPrivate::attribute_names();
    nlohmann::json requestValues;
    requestValues["fields"] = fields;
    session::Response response(std::move(priv_->sendRequest("torrent-get", requestValues)));

    JsonFormat jsonFormat;
    jsonFormat.fromJson(response.get_arguments());
    sequential::from_format(jsonFormat, torrentResponse);
    torrents = torrentResponse.get_torrents();
    for (TorrentPrivate &torrentPriv: torrents)
    {
        auto torrent = new TorrentPrivate(std::move(torrentPriv));
        torrent->session_ = this->priv_;
        retValue.emplace_back(torrent);
    }

    return std::move(retValue);
}

std::string Session::url() const
{
    return priv_->url_;
}

void Session::setUrl(const std::string &url)
{
    priv_->url_ = url;
}

void Session::setUrl(std::string &&url)
{
    priv_->url_ = url;
}

std::string Session::path() const
{
    return priv_->path_;
}

void Session::setPath(const std::string &path)
{
    priv_->path_ = path;
}

void Session::setPath(std::string &&path)
{
    priv_->path_ = path;
}

bool Session::authenticationRequired() const
{
    return priv_->authenticationRequired_;
}

void Session::setAuthentication(Session::Authentication authentication)
{
    priv_->authenticationRequired_ = (authentication == Session::Authentication::Required);
}

std::string Session::username() const
{
    return priv_->username_;
}

void Session::setUsername(const std::string &username)
{
    priv_->username_ = username;
}

void Session::setUsername(std::string &&username)
{
    priv_->username_ = username;
}

void Session::setPassword(const std::string &password)
{
    priv_->password_ = password;
}

void Session::setPassword(std::string &&password)
{
    priv_->password_ = password;
}
