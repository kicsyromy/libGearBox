#include "librt_session.h"

#include <string>
#include "utility"

#include <formats/json_format.h>
#include <cpr/cpr.h>
#include <fmt/format.h>

#include "librt_session_p.h"
#include "librt_torrent_p.h"

#include "librt_logger_p.h"

using nlohmann::json;
using namespace librt;

namespace
{
    constexpr std::uint16_t SESSION_TAG    = 33872;
    constexpr std::uint8_t  RETRY_COUNT    = 5;
    constexpr std::int32_t DEFAULT_TIMEOUT = 5000;

    constexpr std::int32_t STATUS_OK                 = 200;
    constexpr std::int32_t STATUS_RETRY              = 409;
    constexpr std::int32_t STATUS_METHOD_NOT_ALLOWED = 405;

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
                               std::int32_t port,
                               bool authenticationRequired,
                               const std::string &username,
                               const std::string &password) :
    url_(url),
    path_(path),
    port_(port),
    authenticationRequired_(authenticationRequired),
    username_(username),
    password_(password),
    timeout_(DEFAULT_TIMEOUT)
{
}

SessionPrivate::SessionPrivate(std::string &&url,
                               std::string &&path,
                               std::int32_t port,
                               bool authenticationRequired,
                               std::string &&username,
                               std::string &&password) :
    url_(url),
    path_(path),
    port_(port),
    authenticationRequired_(authenticationRequired),
    username_(username),
    password_(password),
    timeout_(DEFAULT_TIMEOUT)
{
}

session::Response SessionPrivate::sendRequest(const std::string &method, nlohmann::json arguments)
{
    session::Request  request(arguments, method, SESSION_TAG);
    session::Response response;
    JsonFormat jsonFormat;
    sequential::to_format(jsonFormat, request);

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
    for (std::uint8_t it = 0; it < RETRY_COUNT; ++it)
    {
        cpr::Response result;
        if (authenticationRequired_)
        {
            result = cpr::Post(fmt::format("{}{}{}{}", url_, path_, port_ > 0 ? ":", fmt::format("{}", port_) : "", ""),
                               cpr::Authentication{username_, password_},
                               cpr::Header{{"X-Transmission-Session-Id", sessionId}},
                               cpr::Timeout{timeout_},
                               cpr::Body(jsonFormat.output().dump()));
        }
        else
        {
            result = cpr::Post(fmt::format("{}{}{}{}", url_, path_, port_ > 0 ? ":", fmt::format("{}", port_) : "", ""),
                               cpr::Header{{"X-Transmission-Session-Id", sessionId}},
                               cpr::Timeout{timeout_},
                               cpr::Body(jsonFormat.output().dump()));
        }

        if (result.error)
        {
            response.error = std::make_pair(
                        static_cast<Error::Code>(result.error.code),
                        std::move(result.error.message)
                    );
            break;
        }

        if (result.status_code == STATUS_RETRY)
        {
            sessionId = getSessionId(result.text);
            continue;
        }

        if (result.status_code == STATUS_METHOD_NOT_ALLOWED)
        {
            response.error = std::make_pair(
                        Error::Code::TransmissionMethodNotAllowed,
                        std::move("Method not allowed")
                    );
            break;
        }

        if (result.status_code == STATUS_OK)
        {
            jsonFormat.parse(result.text);
            sequential::from_format(jsonFormat, response);

            auto &result = response.get_result();
            if (result != "success" && !result.empty())
            {
                response.error = std::make_pair(Error::Code::TransmissionUnknownError, std::move(result));
            }

            break;
        }
        else
        {
            response.error = std::make_pair(Error::Code::TransmissionUnknownError, std::move("Unknown error"));
            break;
        }
    }

    if (response.error)
        LOG_ERROR("Error '{} {}' while issuing method call '{}' with arguments\n'{}'",
                  static_cast<int>(response.error.errorCode()),
                  response.error.message(),
                  method,
                  arguments.dump(4));
    else
        LOG_DEBUG("Method call '{}' result '{}' for tag '{}':\n{}",
                  method,
                  response.get_result(),
                  response.get_tag(),
                  response.get_arguments().dump(4));

    return std::move(response);
}

Session::Session() :
    priv_(new SessionPrivate("", DEFAULT_PATH, -1,  false, "", ""))
{
}

Session::Session(Session &&other) :
    priv_(std::move(other.priv_))
{
}

Session &Session::operator =(Session &&other)
{
    priv_= std::move(other.priv_);
}

Session::Session(const std::string &url,
                 const std::string &path,
                 std::int32_t port,
                 Authentication authentication,
                 const std::string &username,
                 const std::string &password) :
    priv_(new SessionPrivate(url, path, port, authentication ==  Authentication::Required, username, password))
{
}

Session::Session(std::string &&url,
                 std::string &&path,
                 std::int32_t port,
                 Authentication authentication,
                 std::string &&username,
                 std::string &&password) :
    priv_(new SessionPrivate(url, path, port, authentication ==  Authentication::Required, username, password))
{
}

std::int32_t Session::timeout() const
{
    return priv_->timeout_;
}

void Session::setTimeout(std::int32_t value)
{
    priv_->timeout_ = value;
}

ReturnType<Session::Statistics> Session::statistics() const
{
    Session::Statistics retValue;
    session::Response response(std::move(priv_->sendRequest("session-stats")));
    if (!response.error)
    {
        session::Statistics stats;
        JsonFormat jsonFormat(response.get_arguments());
        sequential::from_format(jsonFormat, stats);
        retValue = {
            stats.get_torrentCount(),
            stats.get_activeTorrentCount(),
            stats.get_pausedTorrentCount(),
            stats.get_downloadSpeed(),
            stats.get_uploadSpeed()
        };
    }

    return std::move(
        ReturnType<Session::Statistics>(
            std::move(response.error),
            std::move(retValue)
        )
    );
}

ReturnType<std::vector<Torrent>> Session::torrents() const
{
    std::vector<Torrent> retValue;
    std::vector<TorrentPrivate> torrents;
    TorrentPrivate::Response torrentResponse;

    const auto &fields = TorrentPrivate::attribute_names();
    nlohmann::json requestValues;
    requestValues["fields"] = fields;
    session::Response response(std::move(priv_->sendRequest("torrent-get", requestValues)));

    if (!response.error)
    {
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
    }

    return std::move(
        ReturnType<std::vector<Torrent>>(
            std::move(response.error),
            std::move(retValue)
        )
    );
}

ReturnType<std::vector<std::int32_t>> Session::recentlyRemoved() const
{
    std::vector<std::int32_t> ids;
    TorrentPrivate::Response torrentResponse;

    const auto &fields = TorrentPrivate::attribute_names();
    nlohmann::json requestValues;
    requestValues["ids"] = "recently-active";
    requestValues["fields"] = fields;
    session::Response response(std::move(priv_->sendRequest("torrent-get", requestValues)));

    if (!response.error)
    {
        JsonFormat jsonFormat;
        jsonFormat.fromJson(response.get_arguments());
        sequential::from_format(jsonFormat, torrentResponse);
        ids = torrentResponse.get_removed();
    }

    return std::move(
        ReturnType<std::vector<std::int32_t>>(
            std::move(response.error),
            std::move(ids)
        )
    );
}

Error Session::updateTorrentStats(std::vector<librt::Torrent> &torrents)
{
    std::vector<std::int32_t> ids;
    ids.reserve(torrents.size());
    for (auto &t: torrents)
        ids.push_back(t.id());

    TorrentPrivate::Response torrentResponse;

    const auto &fields = TorrentPrivate::attribute_names();
    nlohmann::json requestValues;
    requestValues["ids"] = ids;
    requestValues["fields"] = fields;
    session::Response response(std::move(priv_->sendRequest("torrent-get", requestValues)));

    if (!response.error)
    {
        JsonFormat jsonFormat;
        jsonFormat.fromJson(response.get_arguments());
        sequential::from_format(jsonFormat, torrentResponse);

        auto &updatedTorrents = torrentResponse.get_torrents();
        for (auto &t: torrents)
        {
            bool found = false;
            for (auto &torrentPriv: updatedTorrents)
            {
                if (torrentPriv.get_id() == t.id())
                {
                    auto torrent = new TorrentPrivate(std::move(torrentPriv));
                    torrent->session_ = this->priv_;
                    t.priv_.reset(torrent);
                    break;
                }
            }
            if (!found)
                t.priv_.reset();
        }
    }

    return std::move(response.error);
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

std::int32_t Session::port() const
{
    return priv_->port_;
}

void Session::setPort(std::int32_t port)
{
    priv_->port_ = port;
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

std::string Session::password() const
{
    return priv_->password_;
}

void Session::setPassword(const std::string &password)
{
    priv_->password_ = password;
}

void Session::setPassword(std::string &&password)
{
    priv_->password_ = password;
}
