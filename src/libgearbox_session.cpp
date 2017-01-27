#include "libgearbox_session.h"

#include <string>
#include <utility>

#include <formats/json_format.h>
#include <fmt/format.h>

#include "libgearbox_session_p.h"
#include "libgearbox_torrent_p.h"
#include "libgearbox_logger_p.h"

using nlohmann::json;
using namespace gearbox;

namespace
{
    constexpr std::uint16_t SESSION_TAG     { 33872 };
    constexpr std::int32_t  DEFAULT_TIMEOUT {  5000 };
    constexpr std::int32_t  RETRY_COUNT     {     5 };
    constexpr const char    USER_AGENT[29]  { "libRemoteTransmission v" LIBGEARBOX_VERISION_STR };
}

SessionPrivate::SessionPrivate(const std::string &url,
                               const std::string &path,
                               std::int32_t port,
                               bool authenticationRequired,
                               const std::string &username,
                               const std::string &password) :
    sessionId_("dummy"),
    http_(USER_AGENT)
{
    http_.setPort(port);
    http_.setHost(url);
    http_.setPath(path);
    http_.setUsername(username);
    http_.setPassword(password);
    http_.setTimeout(std::chrono::milliseconds(DEFAULT_TIMEOUT));
    if (!authenticationRequired)
        http_.disableAuthentication();
}

SessionPrivate::SessionPrivate(std::string &&url,
                               std::string &&path,
                               std::int32_t port,
                               bool authenticationRequired,
                               std::string &&username,
                               std::string &&password) :
    sessionId_("dummy"),
    http_(USER_AGENT)
{
    http_.setPort(port);
    http_.setHost(std::forward<std::string>(url));
    http_.setPath(std::forward<std::string>(path));
    http_.setUsername(std::forward<std::string>(username));
    http_.setPassword(std::forward<std::string>(password));
    http_.setTimeout(std::chrono::milliseconds(DEFAULT_TIMEOUT));
    if (!authenticationRequired)
        http_.disableAuthentication();
}

session::Response SessionPrivate::sendRequest(const std::string &method, nlohmann::json arguments)
{
    session::Request  request(arguments, method, SESSION_TAG);
    session::Response response;
    JsonFormat jsonFormat;
    sequential::to_format(jsonFormat, request);

    LOG_DEBUG("Requesting \"{}\": \n{}", method, jsonFormat.output().dump(4));
    auto r = http_.createRequest();
    r.setHeader({ "Content-Type", "application/json" });
    r.setBody(jsonFormat.output().dump());

    /* As per the Transmission documentation:                                   */
    /* Most Transmission RPC servers require a X-Transmission-Session-Id        */
    /* header to be sent with requests, to prevent CSRF attacks.                */
    /* When your request has the wrong id -- such as when you send your first   */
    /* request, or when the server expires the CSRF token -- the                */
    /* Transmission RPC server will return an HTTP 409 error with the           */
    /* right X-Transmission-Session-Id in its own headers.                      */
    /* So, the correct way to handle a 409 response is to update your           */
    /* X-Transmission-Session-Id and to resend the previous request.            */
    for (std::int32_t it = 0; it < RETRY_COUNT; ++it)
    {
        r.setHeader({ "X-Transmission-Session-Id", sessionId_ });
        auto &&result = r.send();

        if (result.error)
        {
            LOG_DEBUG("Error: {}", static_cast<std::string>(result.error));
            response.error = gearbox::Error(gearbox::Error::Code::TransmissionUnknownError, "Some dummy error until I can figure out how to convert between types");
            break;
        }

        if (result.status == gearbox::http::Status::Conflict)
        {
            sessionId_ = result.response.headers["X-Transmission-Session-Id"];
            continue;
        }

        if (result.status == gearbox::http::Status::MethodNotAllowed)
        {
            response.error = std::make_pair(
                Error::Code::TransmissionMethodNotAllowed,
                std::string{ "Method not allowed" }
            );
            break;
        }

        if (result.status == gearbox::http::Status::OK)
        {
            LOG_DEBUG("{}", result.response.text);
            jsonFormat.parse(result.response.text);
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
            response.error = std::make_pair(Error::Code::TransmissionUnknownError, std::string{ "Unknown error" });
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

    return response;
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

    return *this;
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

ReturnType<Session::Statistics> Session::statistics() const
{
    Session::Statistics retValue;
    session::Response response(priv_->sendRequest("session-stats"));
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

    return ReturnType<Session::Statistics>(
        std::move(response.error),
        std::move(retValue)
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
    session::Response response(priv_->sendRequest("torrent-get", requestValues));

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

    return ReturnType<std::vector<Torrent>>(
        std::move(response.error),
        std::move(retValue)
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
    session::Response response(priv_->sendRequest("torrent-get", requestValues));

    if (!response.error)
    {
        JsonFormat jsonFormat;
        jsonFormat.fromJson(response.get_arguments());
        sequential::from_format(jsonFormat, torrentResponse);
        ids = torrentResponse.get_removed();
    }

    return ReturnType<std::vector<std::int32_t>>(
        std::move(response.error),
        std::move(ids)
    );
}

Error Session::updateTorrentStats(std::vector<std::reference_wrapper<Torrent>> &torrents)
{
    std::vector<std::int32_t> ids;
    ids.reserve(torrents.size());
    for (Torrent &t: torrents)
        ids.push_back(t.id());

    TorrentPrivate::Response torrentResponse;

    const auto &fields = TorrentPrivate::attribute_names();
    nlohmann::json requestValues;
    requestValues["ids"] = ids;
    requestValues["fields"] = fields;
    session::Response response(priv_->sendRequest("torrent-get", requestValues));

    if (!response.error)
    {
        JsonFormat jsonFormat;
        jsonFormat.fromJson(response.get_arguments());
        sequential::from_format(jsonFormat, torrentResponse);

        auto &updatedTorrents = torrentResponse.get_torrents();
        for (Torrent &t: torrents)
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

const std::string &Session::url() const
{
    return priv_->http_.host();
}

void Session::setUrl(const std::string &url)
{
    priv_->http_.setHost(url);
}

void Session::setUrl(std::string &&url)
{
    priv_->http_.setHost(std::forward<std::string>(url));
}

const std::string &Session::path() const
{
    return priv_->http_.path();
}

void Session::setPath(const std::string &path)
{
    priv_->http_.setPath(path);
}

void Session::setPath(std::string &&path)
{
    priv_->http_.setPath(std::forward<std::string>(path));
}

std::int32_t Session::port() const
{
    return priv_->http_.port();
}

void Session::setPort(std::int32_t port)
{
    priv_->http_.setPort(port);
}

bool Session::authenticationRequired() const
{
    return priv_->http_.authenticationRequired();
}

void Session::setAuthentication(Session::Authentication authentication)
{
    if (authentication == Session::Authentication::Required)
    {
        priv_->http_.enableAuthentication();
    }
    else
    {
        priv_->http_.disableAuthentication();
    }
}

const std::string &Session::username() const
{
    return priv_->http_.username();
}

void Session::setUsername(const std::string &username)
{
    priv_->http_.setUsername(username);
}

void Session::setUsername(std::string &&username)
{
    priv_->http_.setUsername(std::forward<std::string>(username));
}

const std::string &Session::password() const
{
    return priv_->http_.password();
}

void Session::setPassword(const std::string &password)
{
    priv_->http_.setPassword(password);
}

void Session::setPassword(std::string &&password)
{
    priv_->http_.setPassword(std::forward<std::string>(password));
}

std::int32_t Session::timeout() const
{
    return static_cast<std::int32_t>(priv_->http_.timeout().count());
}

void Session::setTimeout(std::int32_t value)
{
    priv_->http_.setTimeout(std::chrono::milliseconds(value));
}

void Session::setSSLErrorHandling(Session::SSLErrorHandling value)
{
    priv_->http_.setSSLErrorHandling(static_cast<gearbox::http::SSLErrorHandling>(value));
}
