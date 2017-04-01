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
    \class gearbox::Torrent
    \brief Represents a torrent file on the remote server.

    An valid instance of gearbox::Torrent can only be obtained by calling
    gearbox::Session::torrents.

    It caches some basic information about the torrent such as name, completion,
    ratio, speeds, status, size, etc. More complex information is requested
    on-demand by calling the appropriate functions, see
    gearbox::Torrent::content and gearbox::Torrent::files as examples of this.

    As is with gearbox::Session, all data returned by methods that make an HTTP
    request (i.e. return gearbox::ReturnType or gearbox::Error) are considered
    to be thread safe. Unless otherwise noted, no other methods have this
    guarantee.

    Any and all data returned by these methods should only be considered valid
    as long as the instance of gearbox::Torrent is available.

    In case that the creating gearbox::Session instance is no longer available
    an error code or gearbox::Error::Code::GearboxSessionInvalid will be
    returned.

    Methods are also provided to alter the state of the torrent with regards to
    status, content, peers etc.
*/

#include "libgearbox_torrent.h"

#include <string>
#include <utility>

#include <formats/json_format.h>

#include "libgearbox_torrent_p.h"
#include "libgearbox_session_p.h"
#include "libgearbox_folder.h"
#include "libgearbox_folder_p.h"
#include "libgearbox_logger_p.h"

using namespace gearbox;

namespace
{
    constexpr const char *INVALID_SESSION { "Invalid session" };
    constexpr const char *INVALID_TORRENT { "Invalid torrent" };
}

TorrentPrivate::TorrentPrivate() :
    attributes(),
    session_()
{
}

TorrentPrivate::TorrentPrivate(TorrentPrivate &&other) :
    attributes(std::move(other.attributes)),
    session_(other.session_)
{
    other.session_.reset();
}

TorrentPrivate &TorrentPrivate::operator =(TorrentPrivate &&other)
{
    attributes = std::move(other.attributes);
    session_ = other.session_;
    other.session_.reset();

    return *this;
}

TorrentPrivate::TorrentPrivate(const TorrentPrivate &other) :
    attributes(other.attributes),
    session_(other.session_)
{
}

TorrentPrivate &TorrentPrivate::operator =(const TorrentPrivate &other)
{
    attributes = other.attributes;
    session_ = other.session_;

    return *this;
}

/*!
    \enum gearbox::Torrent::Status
    \brief Enumerates the possible states of a torrent.

    \var gearbox::Torrent::Stopped
    \brief The torrent is stopped.

    \var gearbox::Torrent::CheckWait
    \brief The torrent is queued for file checking.

    \var gearbox::Torrent::Check
    \brief The torrent is checking files.

    \var gearbox::Torrent::DownloadWait
    \brief The torrent is queued for download.

    \var gearbox::Torrent::Download
    \brief The torrent is downloading.

    \var gearbox::Torrent::SeedWait
    \brief The torrent is queued for seeding.

    \var gearbox::Torrent::Seed
    \brief The torrent is seeding.

    \var gearbox::Torrent::Invalid
    \brief The torrent is in an unknown state.
*/

/*!
    \enum gearbox::Torrent::MoveType
    \brief Verbose boolean type used when calling gearbox::Torrent::setDownloadDir()

    \var gearbox::Torrent::MoveToNewLocation
    \brief When passed to gearbox::Torrent::setDownloadDir() moves the currently
    downloaded files to the new location

    \var gearbox::Torrent::SearchForExistingFiles
    \brief When passed to gearbox::Torrent::setDownloadDir() keeps already
    downloaded data as is and searches for existing data in the new location
*/

/*!
    \enum gearbox::Torrent::LocalDataAction
    \brief Verbose boolean type used when calling gearbox::Torrent::remove()

    \var gearbox::Torrent::KeepFiles
    \brief When passed to gearbox::Torrent::remove() the downloaded
    files are keep in the download location

    \var gearbox::Torrent::DeleteFiles
    \brief When passed to gearbox::Torrent::remove() the downloaded
    files are also removed from the download location
*/

/*!
    Constructs an instance based on \c priv parameter. Only used internally
    by gearbox::Session.
*/
Torrent::Torrent(TorrentPrivate *priv) :
    priv_(priv)
{
}

/*!
    Move contructor
*/
Torrent::Torrent(Torrent &&) = default;

/*!
    Move asignment operator
*/
Torrent &Torrent::operator=(Torrent &&) noexcept(true) = default;

Torrent::~Torrent() noexcept(true) = default;

/*!
    Comparison operator
 */
bool Torrent::operator ==(const Torrent &other) const
{
    return valid() ? (priv_->get_id() == other.priv_->get_id()) : false;
}

/*!
    Less than operator
 */
bool Torrent::operator <(const Torrent &other) const
{
    return valid() ? (priv_->get_queuePosition() < other.priv_->get_queuePosition()) : false;
}

/*!
    Returns \c true if the torrent is considered to be valid.

    Validity of a torrent is mostly an implementation detail and could be
    removed from the public interface in the future.
*/
bool Torrent::valid() const
{
    return static_cast<bool>(priv_);
}

/*!
    Starts the torrent if it is stopped. Calling this function will put the
    torrent in one of four states:

    gearbox::Torrent::Status::DownloadWait

    gearbox::Torrent::Status::Download

    gearbox::Torrent::Status::SeedWait

    gearbox::Torrent::Status::Seed

    This method is thread-safe.
*/
Error Torrent::start()
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("torrent-start", request);
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"torrent-start\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"torrent-start\"");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Starts the torrent if it is stopped, ignoring the queue. Calling this
    function will put the torrent in one of two states:

    gearbox::Torrent::Status::Download

    gearbox::Torrent::Status::Seed

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::startNow()
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("torrent-start-now", request);
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"torrent-start-now\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"torrent-start-now\"");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Stops an active torrent. The resulting state will be
    gearbox::Torrent::Status::Stopped.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::stop()
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("torrent-stop", request);
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"torrent-stop\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"torrent-stop\"");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Verifies the integrity of the downloaded data.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::verify()
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("torrent-verify", request);
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"torrent-verify\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"torrent-verify\"");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Normally after a period of time Transmission will ask the associated tracker
    for more peers. This forces this action to happen immediately.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::askForMorePeers()
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("torrent-reannounce", request);
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"torrent-reannounce\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"torrent-reannounce\"");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Removes the torrent from the server. By default the downloaded files will be
    kept. This behavior can be changed by passing
    gearbox::Torrent::LocalDataAction::DeleteFiles.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::remove(LocalDataAction action)
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };
        request["delete-local-data"] = (action == LocalDataAction::DeleteFiles);

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("torrent-remove", request);
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"torrent-remove\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"torrent-remove\"");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Moves up the torrent, by one, in the queue.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::queueMoveUp()
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("queue-move-up", request);
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"queue-move-up\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"queue-move-up\"");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Moves down the torrent, by one, in the queue.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::queueMoveDown()
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("queue-move-down", request);
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"queue-move-down\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"queue-move-down\"");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Moves the torrent to the beginning of the queue.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::queueMoveTop()
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("queue-move-top", request);
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"queue-move-top\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"queue-move-top\"");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Moves the torrent to the end of the queue.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::queueMoveBottom()
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("queue-move-bottom", request);
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"queue-move-bottom\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"queue-move-top\"");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Updates the torrent's cached data.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::update()
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };
        request["fields"] = TorrentPrivate::attribute_names();

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("torrent-get", request);
            error = std::move(response.error);
            if (!error)
            {
                JsonFormat jsonFormat;
                TorrentPrivate::Response torrentResponse;
                std::vector<TorrentPrivate> torrents;

                jsonFormat.fromJson(response.get_arguments());
                sequential::from_format(jsonFormat, torrentResponse);
                torrents = torrentResponse.get_torrents();
                for (TorrentPrivate &torrentPriv: torrents)
                {
                    auto torrent = new TorrentPrivate(std::move(torrentPriv));
                    torrent->session_ = priv_->session_;
                    priv_.reset(torrent);
                }
            }
        }
        else
        {
            LOG_ERROR("Invalid session while requesting update for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting update");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Sets the files that are to be downloaded as a part of this torrent.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::setWantedFiles(const std::vector<std::reference_wrapper<const File>> &files)
{
    Error error;

    if (valid())
    {
        std::vector<std::size_t> indices;
        indices.reserve(files.size());

        for (const File &f: files)
            indices.push_back(f.id_);

        nlohmann::json request;
        request["ids"] = { this->id() };
        request["files-wanted"] = indices;

        if (auto session = priv_->session_.lock())
        {
            error = session->sendRequest("torrent-set", request).error;
        }
        else
        {
            LOG_ERROR("Invalid session while updating wanted files for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while updating wanted files");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Sets the files that are to be skipped as a part of this torrent.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::setSkippedFiles(const std::vector<std::reference_wrapper<const File>> &files)
{
    Error error;

    if (valid())
    {
        std::vector<std::size_t> indices;
        indices.reserve(files.size());

        for (const File &f: files)
            indices.push_back(f.id_);

        nlohmann::json request;
        request["ids"] = { this->id() };
        request["files-unwanted"] = indices;

        if (auto session = priv_->session_.lock())
        {
            error = session->sendRequest("torrent-set", request).error;
        }
        else
        {
            LOG_ERROR("Invalid session while updating unwanted files for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while updating unwanted files");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Returns the unique identifier for the torrent. This value is mostly for
    internal use.

    If the torrent is not gearbox::Torrent::valid() returns -1.
*/
int32_t Torrent::id() const
{
    return valid() ? priv_->get_id() : -1;
}

/*!
    Returns the name of torrent.

    If the torrent is not gearbox::Torrent::valid() returns an empty string.
*/
std::string Torrent::name() const
{
    return valid() ? priv_->get_name() : "";
}

/*!
    Returns the amount of downloaded data in bytes.

    If the torrent is not gearbox::Torrent::valid() returns 0.
*/
uint64_t Torrent::bytesDownloaded() const
{
    return valid() ? priv_->get_haveValid() : 0;
}

/*!
    Returns how much has been downloaded of the wanted
    (gearbox::Torrent::setWantedFiles(), gearbox::Torrent::setSkippedFiles()),
    files.

    If the torrent is not gearbox::Torrent::valid() returns 0.
*/
double Torrent::percentDone() const
{
    return valid() ? priv_->get_percentDone() : 0;
}

/*!
    Returns the amount of uploaded data devided by the amount of uploaded data
    as a value between 0.0 and 1.0.

    If the torrent is not gearbox::Torrent::valid() returns 0.
*/
double Torrent::uploadRatio() const
{
    return valid() ? priv_->get_uploadRatio() : 0;
}

/*!
    Returns the amount of uploaded data in bytes.

    If the torrent is not gearbox::Torrent::valid() returns 0.
*/
std::uint64_t Torrent::bytesUploaded() const
{
    return valid() ? priv_->get_uploadedEver() : 0;
}

/*!
    Returns the speed at which the torrent is downloading in bytes/s.

    If the torrent is not gearbox::Torrent::valid() returns 0.
*/
uint64_t Torrent::downloadSpeed() const
{
    return valid() ? priv_->get_rateDownload() : 0;
}

/*!
    Returns the speed at which the torrent is uploading in bytes/s.

    If the torrent is not gearbox::Torrent::valid() returns 0.
*/
uint64_t Torrent::uploadSpeed() const
{
    return valid() ? priv_->get_rateUpload() : 0;
}

/*!
    Returns current status of the torrent.

    If the torrent is not gearbox::Torrent::valid() returns
    gearbox::Torrent::Status::Invalid.
*/
Torrent::Status Torrent::status() const
{
    return valid() ? static_cast<Torrent::Status>(priv_->get_status()) : Torrent::Status::Invalid;
}

/*!
    Returns occupied space by the data once download completes.

    If the torrent is not gearbox::Torrent::valid() returns 0.
*/
uint64_t Torrent::size() const
{
    return valid() ? priv_->get_totalSize() : 0;
}

/*!
    Returns "estimated time of arrival" for the torrent. If the torrent is
    downloading, estimated number of seconds left until the torrent is done.
    If the torrent is seeding, estimated number of seconds left until seed
    ratio is reached.

    If the torrent is not gearbox::Torrent::valid() returns 0.
*/
int32_t Torrent::eta() const
{
    return valid() ? priv_->get_eta() : 0;
}

/*!
    Returns the root folder of the torrent.

    In it's current incarnation this method can be expensive to call since it
    creates the tree structure, on the client-side, for each subsequent call.

    The lifetime and validity of the returned folder is not tied in any way to
    the lifetime of the torrent, but the former should be considered invalid or,
    at the very least, stale if the torrent is removed from the server.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
ReturnType<Folder> Torrent::content() const
{
    Folder result((std::string(name())));
    Error error;

    if (valid())
    {
        std::vector<TorrentPrivate::Files> torrentFiles;
        TorrentPrivate::Files::Response torrentResponse;

        TorrentPrivate::Files::Request fileRequest;
        fileRequest.set_ids({this->id()});
        JsonFormat jsonFileRequest;
        sequential::to_format(jsonFileRequest, fileRequest);

        if (auto session = priv_->session_.lock())
        {
            session::Response response(session->sendRequest("torrent-get", jsonFileRequest.output()));

            if (!response.error)
            {
                JsonFormat jsonFormat;
                jsonFormat.fromJson(response.get_arguments());
                sequential::from_format(jsonFormat, torrentResponse);
                torrentFiles  = torrentResponse.get_torrents();
                for (TorrentPrivate::Files &tf: torrentFiles)
                {
                    const auto &files = tf.get_files();
                    const auto &fileStats = tf.get_fileStats();
                    const auto length = files.size();

                    for (std::size_t it = 0; it < length; ++it)
                    {
                        result.priv_->addPath(
                            files.at(it).get_name(),
                            it,
                            files.at(it).get_bytesCompleted(),
                            files.at(it).get_length(),
                            fileStats.at(it).get_wanted(),
                            static_cast<File::Priority>(fileStats.at(it).get_priority())
                        );
                    }
                }
            }
            else
            {
                error = std::move(response.error);
            }
        }
        else
        {
            LOG_ERROR("Invalid session while requesting file list for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting file list");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return ReturnType<Folder> {
        std::move(error),
        std::move(result)
    };
}

/*!
    Returns a list of files that the torrent can download.

    Each file will be in the form of
    `<gearbox::Torrent::name>/subfolder/../file_name.ext`

    The lifetime and validity of the returned files is not tied in any way to
    the lifetime of the torrent, but the former should be considered invalid or,
    at the very least, stale if the torrent is removed from the server.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
ReturnType<std::vector<File>> Torrent::files() const
{
    std::vector<File> result;
    Error error;

    if (valid())
    {
        std::vector<TorrentPrivate::Files> torrentFiles;
        TorrentPrivate::Files::Response torrentResponse;

        TorrentPrivate::Files::Request fileRequest;
        fileRequest.set_ids({this->id()});
        JsonFormat jsonFileRequest;
        sequential::to_format(jsonFileRequest, fileRequest);

        if (auto session = priv_->session_.lock())
        {
            session::Response response(session->sendRequest("torrent-get", jsonFileRequest.output()));

            if (!response.error)
            {
                JsonFormat jsonFormat;
                jsonFormat.fromJson(response.get_arguments());
                sequential::from_format(jsonFormat, torrentResponse);
                torrentFiles  = torrentResponse.get_torrents();
                for (TorrentPrivate::Files &tf: torrentFiles)
                {
                    auto &files = tf.get_files();
                    const auto &fileStats = tf.get_fileStats();
                    const auto length = files.size();

                    for (std::size_t it = 0; it < length; ++it)
                    {
                        File f {
                            std::move(files.at(it).get_name()),
                            files.at(it).get_bytesCompleted(),
                            files.at(it).get_length(),
                            fileStats.at(it).get_wanted(),
                            static_cast<File::Priority>(fileStats.at(it).get_priority()),
                        };
                        f.id_ = it;
                        result.push_back(std::move(f));
                    }
                }
            }
            else
            {
                error = std::move(response.error);
            }
        }
        else
        {
            LOG_ERROR("Invalid session while requesting file list for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting file list");
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return ReturnType<std::vector<File>>(
        std::move(error),
        std::move(result)
    );
}

/*!
    Returns the position in the queue for this torrent.

    This value will be between 0 and
    gearbox::Session::Statistics::totalTorrentCount since all torrents have a
    queue position even if they aren't queued.

    If the torrent is not gearbox::Torrent::valid() returns -1.
*/
int32_t Torrent::queuePosition() const
{
    return valid() ? priv_->get_queuePosition() : -1;
}

/*!
    Sets the position, in the queue, for the torrent.

    The supplied value should be no larger than
    gearbox::Session::Statistics::totalTorrentCount and no smaller than 0.
    Passing a value not in thsi range leads to undefined behaviour.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::setQueuePosition(int32_t position)
{
    Error error;

    if (valid())
    {
        nlohmann::json request;
        request["ids"] = { this->id() };
        request["queuePosition"] = position;

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("torrent-set", request);
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"torrent-set\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"torrent-set\" with queue-position '{}'", position);
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}

/*!
    Returns the target directory for the torrent.

    If the torrent is not gearbox::Torrent::valid() returns an empty string.
*/
std::string Torrent::downloadDir() const
{
    return valid() ? priv_->get_downloadDir() : "";
}

/*!
    Sets the target directory for the torrent.

    Calling this method depends on the fact that the gearbox::Session that
    returned it is still available, otherwise it will return
    gearbox::Error::Code::GearboxSessionInvalid.

    This method is thread-safe.
*/
Error Torrent::setDownloadDir(const std::string &path, MoveType move)
{
    Error error;

    if (valid())
    {
        JsonFormat jsonFormat;
        TorrentPrivate::MoveRequest moveRequest;
        moveRequest.set_ids({this->id()});
        moveRequest.set_location(path);
        moveRequest.set_move((move == Torrent::MoveType::MoveToNewLocation) ? true : false);
        sequential::to_format(jsonFormat, moveRequest);

        if (auto session = priv_->session_.lock())
        {
            auto response = session->sendRequest("torrent-set-location", jsonFormat.output());
            error = std::move(response.error);
        }
        else
        {
            LOG_ERROR("Invalid session while requesting \"torrent-set-location\" for id '{}'", this->id());
            error = std::make_pair(Error::Code::GearboxSessionInvalid, INVALID_SESSION);
        }
    }
    else
    {
        LOG_ERROR("Invalid torrent while requesting \"torrent-set-location\" to '{}'", path);
        error = std::make_pair(Error::Code::GearboxTorrentInvalid, INVALID_TORRENT);
    }

    return error;
}
