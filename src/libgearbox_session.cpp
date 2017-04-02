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

/*!
    \class gearbox::Session
    \brief Represents a connection to a remote server.

    An instance of this class is used to establish a connection to a
    remote server. It is the main interface through which any and all
    communication with said server happens.

    All data that is returned by a call that has a return type of type
    gearbox::ReturnType or gearbox::Error should be considered invalid or stale
    after the destruction of the instance on which the call was initiated. That
    is using this data after the instance of gearbox::Session has be destroyed
    is considered incorrect usage and the appropriate
    gearbox::Error::Code::GearboxSessionInvalid will be issued where applicable.

    The instance of gearbox::Session only stores the information that it
    gets passed. The only point in which the information is used is when a
    request is sent, at which point the information is copied internally for
    each request. This has the advantage that several requests can be made in
    parallel (different threads) with different settings
    (URL, authentication, etc.).

    All methods that create HTTP requests (that return gearbox::ReturnType or
    gearbox::Error) are thread-safe this means that it is possible, and
    encouraged, to call these methods from a different thread to avoid blocking
    the main thread, and, as a result, any UI the application may have. No such
    guarantees are made about any other methods that the class has.

    Unless they fall into the above category or otherwise noted, methods should
    \b not be considered thread-safe.

    Example (without error checking):
    ```
    gearbox::Session session {
        "https://mytransmissionserver.com",
        gearbox::Session::DEFAULT_PATH,
        gearbox::Session::PORT_AUTODETECT,
        gearbox::Session::Authentication::Required,
        "username",
        "password"
    };

    auto torrents = session.torrents();
    std::cerr << "Got " << torrents.value.size() << " torrents:\n";
    for (const auto &torrent: torrents.value)
    {
        std::cerr << "\tName: " << torrent.name() << '\n';
        auto files = torrent.files();
        for (const auto &file : files.value)
        {
            std::cerr << "\t\tName: " << file.name() << '\n';
            std::cerr << "\t\tMIMEType: " << file.type().name << '\n';
        }
    }
    ```
*/

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
    constexpr const char    USER_AGENT[17]  { "libGearbox/" LIBGEARBOX_VERISION_STR };
}

SessionPrivate::SessionPrivate(const std::string &host,
                               const std::string &path,
                               std::int32_t port,
                               bool authenticationRequired,
                               const std::string &username,
                               const std::string &password) :
    sessionId_("dummy"),
    http_(USER_AGENT)
{
    http_.setHost(host);
    http_.setPath(path);
    http_.setPort(port);
    http_.setUsername(username);
    http_.setPassword(password);
    http_.setTimeout(std::chrono::milliseconds(DEFAULT_TIMEOUT));
    if (!authenticationRequired)
        http_.disableAuthentication();
}

SessionPrivate::SessionPrivate(std::string &&host,
                               std::string &&path,
                               std::int32_t port,
                               bool authenticationRequired,
                               std::string &&username,
                               std::string &&password) :
    sessionId_("dummy"),
    http_(USER_AGENT)
{
    http_.setHost(std::move(host));
    http_.setPath(std::move(path));
    http_.setPort(port);
    http_.setUsername(std::move(username));
    http_.setPassword(std::move(password));
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
            response.error = Error(static_cast<Error::Code>(result.error.errorCode), result.error.message);
            break;
        }

        if (result.status == gearbox::http::Status::Conflict)
        {
            sessionId_ = result.response.headers["X-Transmission-Session-Id"];
            continue;
        }

        if (result.status == gearbox::http::Status::OK)
        {
            LOG_DEBUG("{}", result.response.text);
            jsonFormat.parse(result.response.text);
            sequential::from_format(jsonFormat, response);

            auto &result = response.get_result();
            if (result != "success" && !result.empty())
            {
                response.error = std::make_pair(Error::Code::UnknownError, std::move(result));
            }

            break;
        }
        else
        {
            response.error = std::make_pair(static_cast<Error::Code>(result.status.code()), result.status.name());
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

/*!
    \var gearbox::Session::DEFAULT_PATH
    \brief A string constant that holds the default path for a Transmission server.
*/

/*!
    \var gearbox::Session::PORT_AUTODETECT
    \brief A flag that tells the underlying HTTP client to try and guess the
    port used for the connection.
*/

/*!
    \enum gearbox::Session::Authentication
    \brief Verbose boolean type for authentication requirement

    \var gearbox::Session::Required
    \brief The server requires HTTP Basic Authentication

    \var gearbox::Session::None
    \brief The server requires no authentication
*/

/*!
    \enum gearbox::Session::SSLErrorHandling
    \brief Verbose boolean type for SSL related error handling

    \var gearbox::Session::Acknowledge
    \brief When passed to gearbox::Session::setSSLErrorHandling, all SSL-related errors are reported

    \var gearbox::Session::Ignore
    \brief When passed to gearbox::Session::setSSLErrorHandling, all SSL-related errors are ignored 
*/

/*!
    \class gearbox::Session::Statistics
    \brief Represents a collection of statistics from the server
*/

/*!
    \var gearbox::Session::Statistics::totalTorrentCount

    The total number of torrents, in all states; stopped, checking, downloading,
    seeding, queued.
*/


/*!
    \var gearbox::Session::Statistics::activeTorrentCount
    
    The number of active torrents; downloading or seeding.
*/

/*!
    \var gearbox::Session::Statistics::pausedTorrentCount

    The number of paused torrents.
*/

/*!
    \var gearbox::Session::Statistics::downloadSpeed

    The overall download speed, in bytes
*/

/*!
    \var gearbox::Session::Statistics::uploadSpeed

    The overall upload speed, in bytes.
*/

/*!
    Constructs an empty gearbox::Session
*/
Session::Session() :
    priv_(new SessionPrivate("", DEFAULT_PATH, -1,  false, "", ""))
{
}

/*!
    Move constructor
*/
Session::Session(Session &&other) :
    priv_(std::move(other.priv_))
{
}

/*!
    Move asignment operator
*/
Session &Session::operator =(Session &&other)
{
    priv_= std::move(other.priv_);

    return *this;
}

/*!
    Constructs a gearbox::Session with the supplied information

    All parameters, except for the \c host, are optional.
    If passing a \c username and \c password but setting \c authentication to
    gearbox::Session::Authentication::None they will be stored but ignored.
    This means that you can turn on authentication later via
    gearbox::Session::setAuthentication and the cached \c username and
    \c password are used.

    Unless specified, the \c port is automatically detected from the supplied
    \c host parameter, 443 for HTTPS and 80 otherwise. This behaviour can be
    enforced by passing gearbox::Session::PORT_AUTODETECT as this parameter.

    For more information about the format in which the host and path are
    expected see their respective documentation, gearbox::Session::setHost and
    gearbox::Session::setPath
*/
Session::Session(const std::string &host,
                 const std::string &path,
                 std::int32_t port,
                 Authentication authentication,
                 const std::string &username,
                 const std::string &password) :
    priv_(new SessionPrivate(host, path, port, authentication ==  Authentication::Required, username, password))
{
}

/*!
    Constructs a gearbox::Session with the supplied information

    All parameters, except for the \c host, are optional.
    If passing a \c username and \c password but setting \c authentication to
    gearbox::Session::Authentication::None they will be stored but ignored.
    This means that you can turn on authentication later via
    gearbox::Session::setAuthentication and the cached \c username and
    \c password are used.

    Unless specified, the \c port is automatically detected from the supplied
    \c host parameter, 443 for HTTPS and 80 otherwise. This behaviour can be
    enforced by passing gearbox::Session::PORT_AUTODETECT as this parameter.

    For more information about the format in which the host and path are
    expected see their respective documentation, gearbox::Session::setHost and
    gearbox::Session::setPath

    This constructor moves the data into the instance, instead of copying it
    as the previous one.
*/
Session::Session(std::string &&host,
                 std::string &&path,
                 std::int32_t port,
                 Authentication authentication,
                 std::string &&username,
                 std::string &&password) :
    priv_(new SessionPrivate(host, path, port, authentication ==  Authentication::Required, username, password))
{
}

/*!
    Returns a set of statistics from the server.

    This method is thread-safe.
*/
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

/*!
    Returns a list of all active and inactive torrents on the server.

    The lifetime of these objects is tied to the gearbox::Session wich returns
    them.

    This method is thread-safe.
*/
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

/*!
    Returns a list of gearbox::Torrent::id that were "recently" removed from
    the server.

    This method is thread-safe.
*/
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

/*!
    Updates the data associated with the supplied gearbox::Torrent(s).

    This method is thread-safe.
*/
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
                    found = true;
                    break;
                }
            }
            if (!found)
                t.priv_.reset();
        }
    }

    return std::move(response.error);
}

/*!
    Returns the host asociated with the session.

    On Windows this does not contain the associated protocol string i.e.
    `http://` or `https://`.
*/
const std::string &Session::host() const
{
    return priv_->http_.host();
}

/*!
    Sets the host for future requests. Does not affect any ongoing requests.

    A proper host is in the form of `http(s)://<hostname>` or 
    `http(s)://<ip-address>` without a terminating \c /. If the protocol is
    ommited HTTP is assumed.
*/
void Session::setHost(const std::string &url)
{
    priv_->http_.setHost(url);
}

/*!
    Sets the host for future requests. Does not affect any ongoing requests.

    A proper host is in the form of `http(s)://<hostname>` or 
    `http(s)://<ip-address>` without a terminating \c /. If the protocol is
    ommited HTTP is assumed.
*/
void Session::setHost(std::string &&url)
{
    priv_->http_.setHost(std::forward<std::string>(url));
}

/*!
    Returns the path associated with the session.
*/
const std::string &Session::path() const
{
    return priv_->http_.path();
}

/*!
    Sets the path that forms the final URL when making requests to the server.

    A path always starts with a \c / and consists of at least this character.

    Setting a host of \c http://192.168.0.50 and a path to \c
    /my/transmission/daemon will form the final URL as: \c
    http://192.168.0.50/my/transmission/daemon
*/
void Session::setPath(const std::string &path)
{
    priv_->http_.setPath(path);
}

/*!
    Sets the path that forms the final URL when making requests to the server.

    A path always starts with a \c / and consists of at least this character.

    Setting a host of \c http://192.168.0.50 and a path to \c
    /my/transmission/daemon will form the final URL as: \c
    http://192.168.0.50/my/transmission/daemon
*/
void Session::setPath(std::string &&path)
{
    priv_->http_.setPath(std::forward<std::string>(path));
}

/*!
    Returns the port associated with the session.
*/
std::int32_t Session::port() const
{
    return priv_->http_.port();
}

/*!
    Sets the port used when making requests to the server.

    If gearbox::Session::PORT_AUTODETECT is used then the port is detected
    automatically based on the host.
*/
void Session::setPort(std::int32_t port)
{
    priv_->http_.setPort(port);
}

/*!
    Returns whether the session is set up to use authentication information.
*/
bool Session::authenticationRequired() const
{
    return priv_->http_.authenticationRequired();
}

/*!
    Sets whether the session should use aouthentication when making requests.
*/
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

/*!
    Returns the username used when making requests.
*/
const std::string &Session::username() const
{
    return priv_->http_.username();
}

/*!
    Sets the username that is to be used when making requests.

    Calling this will automatically enable authentication for any new request
    made afterwards.
*/
void Session::setUsername(const std::string &username)
{
    priv_->http_.setUsername(username);
}

/*!
    Sets the username that is to be used when making requests.

    Calling this will automatically enable authentication for any new request
    made afterwards.
*/
void Session::setUsername(std::string &&username)
{
    priv_->http_.setUsername(std::forward<std::string>(username));
}

/*!
    Returns the password used when making requests.
*/
const std::string &Session::password() const
{
    return priv_->http_.password();
}

/*!
    Sets the password that is to be used when making requests.

    Calling this will automatically enable authentication for any new request
    made afterwards.
*/
void Session::setPassword(const std::string &password)
{
    priv_->http_.setPassword(password);
}

/*!
    Sets the password that is to be used when making requests.

    Calling this will automatically enable authentication for any new request
    made afterwards.
*/
void Session::setPassword(std::string &&password)
{
    priv_->http_.setPassword(std::forward<std::string>(password));
}

/*!
    Returns the timeout value when making requests.
*/
std::int32_t Session::timeout() const
{
    return static_cast<std::int32_t>(priv_->http_.timeout().count());
}

/*!
    Sets the timeout value that is to be used when making requests.

    If any request takes longer, for any reason, than this value an error value
    of gearbox::Error::Code::RequestOperationTimedOut will be returned.
*/
void Session::setTimeout(std::int32_t value)
{
    priv_->http_.setTimeout(std::chrono::milliseconds(value));
}

/*!
    Sets the timeout value that is to be used when making requests.

    If any request takes longer, for any reason, than this value an error value
    of gearbox::Error::Code::RequestOperationTimedOut will be returned.
*/
void Session::setSSLErrorHandling(Session::SSLErrorHandling value)
{
    priv_->http_.setSSLErrorHandling(static_cast<gearbox::http::SSLErrorHandling>(value));
}
