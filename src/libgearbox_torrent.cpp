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

    It caches some basic information about the torrent such as name, completion data,
    ratio, speeds, status, size, etc. More complex information is requested on-demand
    by calling the appropriate functions, see gearbox::Torrent::content() const,
    gearbox::Torrent::files() const.

    It also provides methods to alter the state of the torrent with regards to status,
    content, peers etc.
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

Torrent::Torrent(TorrentPrivate *priv) :
    priv_(priv)
{
}

Torrent::Torrent(Torrent &&) = default;
Torrent &Torrent::operator=(Torrent &&) noexcept(true) = default;
Torrent::~Torrent() noexcept(true) = default;

bool Torrent::operator ==(const Torrent &other) const
{
    return valid() ? (priv_->get_id() == other.priv_->get_id()) : false;
}

bool Torrent::operator <(const Torrent &other) const
{
    return valid() ? (priv_->get_queuePosition() < other.priv_->get_queuePosition()) : false;
}

bool Torrent::valid() const
{
    return static_cast<bool>(priv_);
}

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

int32_t Torrent::id() const
{
    return valid() ? priv_->get_id() : -1;
}

std::string Torrent::name() const
{
    return valid() ? priv_->get_name() : "";
}

uint64_t Torrent::bytesDownloaded() const
{
    return valid() ? priv_->get_haveValid() : 0;
}

double Torrent::percentDone() const
{
    /* Workaround misterious bug where double values end up being truncated */
    return valid() ? static_cast<double>(priv_->get_haveValid()) / priv_->get_totalSize() : 0;
}

double Torrent::uploadRatio() const
{
    return valid() ? priv_->get_uploadRatio() : 0;
}

std::uint64_t Torrent::bytesUploaded() const
{
    return valid() ? priv_->get_uploadedEver() : 0;
}

uint64_t Torrent::downloadSpeed() const
{
    return valid() ? priv_->get_rateDownload() : 0;
}

uint64_t Torrent::uploadSpeed() const
{
    return valid() ? priv_->get_rateUpload() : 0;
}

Torrent::Status Torrent::status() const
{
    return valid() ? static_cast<Torrent::Status>(priv_->get_status()) : Torrent::Status::Invalid;
}

uint64_t Torrent::size() const
{
    return valid() ? priv_->get_totalSize() : 0;
}

int32_t Torrent::eta() const
{
    return valid() ? priv_->get_eta() : 0;
}

/*!
    Returns the root folder of the torrent.

    The lifetime and validity of the returned folder
    is not tied in any way to the lifetime of the torrent, but the former should be considered
    invalid/stale if the torrent is removed from the server.
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

int32_t Torrent::queuePosition() const
{
    return valid() ? priv_->get_queuePosition() : -1;
}

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

std::string Torrent::downloadDir() const
{
    return valid() ? priv_->get_downloadDir() : "";
}

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
