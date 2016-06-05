#include "librt_torrent.h"

#include <formats/json_format.h>

#include "librt_torrent_p.h"
#include "librt_session_p.h"
#include "librt_logger_p.h"

using namespace librt;

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

void Torrent::start()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    if (auto session = priv_->session_.lock())
        session->sendRequest("torrent-start", request);
    else
        LOG_ERROR("Invalid session while requesting \"torrent-start\" for id '{}'", this->id());
}

void Torrent::startNow()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    if (auto session = priv_->session_.lock())
        session->sendRequest("torrent-start-now", request);
    else
        LOG_ERROR("Invalid session while requesting \"torrent-start-now\" for id '{}'", this->id());
}

void Torrent::stop()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    if (auto session = priv_->session_.lock())
        session->sendRequest("torrent-stop", request);
    else
        LOG_ERROR("Invalid session while requesting \"torrent-stop\" for id '{}'", this->id());
}

void Torrent::verify()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    if (auto session = priv_->session_.lock())
        session->sendRequest("torrent-verify", request);
    else
        LOG_ERROR("Invalid session while requesting \"torrent-verify\" for id '{}'", this->id());
}

void Torrent::askForMorePeers()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    if (auto session = priv_->session_.lock())
        session->sendRequest("torrent-reannounce", request);
    else
        LOG_ERROR("Invalid session while requesting \"torrent-reannounce\" for id '{}'", this->id());
}

void Torrent::remove(LocalDataAction action)
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    request["delete-local-data"] = (action == LocalDataAction::DeleteFiles);
    if (auto session = priv_->session_.lock())
        session->sendRequest("torrent-remove", request);
    else
        LOG_ERROR("Invalid session while requesting \"torrent-remove\" for id '{}'", this->id());
}

void Torrent::queueMoveUp()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    if (auto session = priv_->session_.lock())
        session->sendRequest("queue-move-up", request);
    else
        LOG_ERROR("Invalid session while requesting \"queue-move-up\" for id '{}'", this->id());
}

void Torrent::queueMoveDown()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    if (auto session = priv_->session_.lock())
        session->sendRequest("queue-move-down", request);
    else
        LOG_ERROR("Invalid session while requesting \"queue-move-down\" for id '{}'", this->id());
}

void Torrent::queueMoveTop()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    if (auto session = priv_->session_.lock())
        session->sendRequest("queue-move-top", request);
    else
        LOG_ERROR("Invalid session while requesting \"queue-move-top\" for id '{}'", this->id());
}

void Torrent::queueMoveBottom()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    if (auto session = priv_->session_.lock())
        session->sendRequest("queue-move-bottom", request);
    else
        LOG_ERROR("Invalid session while requesting \"queue-move-bottom\" for id '{}'", this->id());
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

const std::vector<Torrent::File> Torrent::files() const
{
    std::vector<Torrent::File> result;
    const auto &files = priv_->get_files();
    const auto &fileStats = priv_->get_fileStats();
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

    return std::move(result);
}

int32_t Torrent::queuePosition() const
{
    return priv_->get_queuePosition();
}

void Torrent::setQueuePosition(int32_t position)
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    request["queuePosition"] = position;
    if (auto session = priv_->session_.lock())
        session->sendRequest("torrent-set", request);
    else
        LOG_ERROR("Invalid session while requesting \"torrent-set\" for id '{}'", this->id());
}

std::string Torrent::downloadDir() const
{
    return priv_->get_downloadDir();
}

void Torrent::setDownloadDir(const std::string &path, MoveType move)
{
    JsonFormat jsonFormat;
    TorrentPrivate::MoveRequest moveRequest;
    moveRequest.set_ids({this->id()});
    moveRequest.set_location(path);
    moveRequest.set_move((move == Torrent::MoveType::MoveToNewLocation) ? true : false);
    sequential::to_format(jsonFormat, moveRequest);

    if (auto session = priv_->session_.lock())
        session->sendRequest("torrent-set-location", jsonFormat.output());
    else
        LOG_ERROR("Invalid session while requesting \"torrent-set-location\" for id '{}'", this->id());
}
