#include "librt_torrent.h"

#include <string>
#include <utility>

#include <formats/json_format.h>

#include "librt_torrent_p.h"
#include "librt_session_p.h"
#include "librt_logger_p.h"

using namespace librt;

namespace
{
    constexpr const char *INVALID_SESSION = "Invalid session";
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

Torrent::Torrent(TorrentPrivate *priv) :
    priv_(priv)
{
}

bool Torrent::operator ==(const Torrent &other) const
{
    return (priv_->get_id() == other.priv_->get_id());
}

bool Torrent::operator <(const Torrent &other) const
{
    return (priv_->get_queuePosition() < other.priv_->get_queuePosition());
}

Error Torrent::start()
{
    nlohmann::json request;
    request["ids"] = { this->id() };

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response = std::move(session->sendRequest("torrent-start", request));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"torrent-start\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}

Error Torrent::startNow()
{
    nlohmann::json request;
    request["ids"] = { this->id() };

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response = std::move(session->sendRequest("torrent-start-now", request));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"torrent-start-now\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}

Error Torrent::stop()
{
    nlohmann::json request;
    request["ids"] = { this->id() };

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response = std::move(session->sendRequest("torrent-stop", request));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"torrent-stop\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}

Error Torrent::verify()
{
    nlohmann::json request;
    request["ids"] = { this->id() };

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response = std::move(session->sendRequest("torrent-verify", request));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"torrent-verify\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}

Error Torrent::askForMorePeers()
{
    nlohmann::json request;
    request["ids"] = { this->id() };

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response = std::move(session->sendRequest("torrent-reannounce", request));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"torrent-reannounce\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}

Error Torrent::remove(LocalDataAction action)
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    request["delete-local-data"] = (action == LocalDataAction::DeleteFiles);

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response = std::move(session->sendRequest("torrent-remove", request));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"torrent-remove\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}

Error Torrent::queueMoveUp()
{
    nlohmann::json request;
    request["ids"] = { this->id() };

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response =std::move(session->sendRequest("queue-move-up", request));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"queue-move-up\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}

Error Torrent::queueMoveDown()
{
    nlohmann::json request;
    request["ids"] = { this->id() };

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response = std::move(session->sendRequest("queue-move-down", request));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"queue-move-down\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}

Error Torrent::queueMoveTop()
{
    nlohmann::json request;
    request["ids"] = { this->id() };

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response = std::move(session->sendRequest("queue-move-top", request));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"queue-move-top\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}

Error Torrent::queueMoveBottom()
{
    nlohmann::json request;
    request["ids"] = { this->id() };

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response = std::move(session->sendRequest("queue-move-bottom", request));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"queue-move-bottom\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}

int32_t Torrent::id() const
{
    return priv_->get_id();
}

std::string Torrent::name() const
{
    return priv_->get_name();
}

uint64_t Torrent::bytesDownloaded() const
{
    return priv_->get_haveValid();
}

double Torrent::percentDone() const
{
    return priv_->get_percentDone();
}

double Torrent::uploadRatio() const
{
    return priv_->get_uploadRatio();
}

std::uint64_t Torrent::bytesUploaded() const
{
    return priv_->get_uploadedEver();
}

uint64_t Torrent::downloadSpeed() const
{
    return priv_->get_rateDownload();
}

uint64_t Torrent::uploadSpeed() const
{
    return priv_->get_rateUpload();
}

Torrent::Status Torrent::status() const
{
    return static_cast<Torrent::Status>(priv_->get_status());
}

uint64_t Torrent::size() const
{
    return priv_->get_totalSize();
}

int32_t Torrent::eta() const
{
    return priv_->get_eta();
}

ReturnType<std::vector<Torrent::File>> Torrent::files() const
{
    std::vector<Torrent::File> result;
    std::vector<TorrentPrivate::Files> torrentFiles;
    TorrentPrivate::Files::Response torrentResponse;

    TorrentPrivate::Files::Request fileRequest;
    fileRequest.set_ids({this->id()});
    JsonFormat jsonFileRequest;
    sequential::to_format(jsonFileRequest, fileRequest);

    Error error;
    if (auto session = priv_->session_.lock())
    {
        session::Response response(std::move(session->sendRequest("torrent-get", jsonFileRequest.output())));

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
                    result.emplace_back(
                        files.at(it).get_name(),
                        files.at(it).get_bytesCompleted(),
                        files.at(it).get_length(),
                        fileStats.at(it).get_wanted(),
                        static_cast<Torrent::File::Priority>(fileStats.at(it).get_priority())
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
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return std::move(
        ReturnType<std::vector<Torrent::File>>(
            std::move(error),
            std::move(result)
        )
    );
}

int32_t Torrent::queuePosition() const
{
    return priv_->get_queuePosition();
}

Error Torrent::setQueuePosition(int32_t position)
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    request["queuePosition"] = position;

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response = std::move(session->sendRequest("torrent-set", request));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"torrent-set\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}

std::string Torrent::downloadDir() const
{
    return priv_->get_downloadDir();
}

Error Torrent::setDownloadDir(const std::string &path, MoveType move)
{
    JsonFormat jsonFormat;
    TorrentPrivate::MoveRequest moveRequest;
    moveRequest.set_ids({this->id()});
    moveRequest.set_location(path);
    moveRequest.set_move((move == Torrent::MoveType::MoveToNewLocation) ? true : false);
    sequential::to_format(jsonFormat, moveRequest);

    Error error;
    if (auto session = priv_->session_.lock())
    {
        auto response = std::move(session->sendRequest("torrent-set-location", jsonFormat.output()));
        error = std::move(response.error);
    }
    else
    {
        LOG_ERROR("Invalid session while requesting \"torrent-set-location\" for id '{}'", this->id());
        error = std::make_pair(Error::Code::libRTInvalidSession, INVALID_SESSION);
    }

    return error;
}
