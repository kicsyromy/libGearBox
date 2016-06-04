#include "librt_torrent.h"

#include <formats/json_format.h>

#include "librt_torrent_p.h"
#include "librt_session_p.h"
#include "librt_logger_p.h"

using namespace librt;

TorrentPrivate::TorrentPrivate() :
    attributes()
{
}

TorrentPrivate::TorrentPrivate(TorrentPrivate &&other) :
    attributes(std::move(other.attributes))
{
}

TorrentPrivate &TorrentPrivate::operator =(TorrentPrivate &&other)
{
    attributes = std::move(other.attributes);
    return *this;
}

TorrentPrivate::TorrentPrivate(const TorrentPrivate &other) :
    attributes(other.attributes)
{
}

TorrentPrivate &TorrentPrivate::operator =(const TorrentPrivate &other)
{
    attributes = other.attributes;
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
    session::sendRequest(request, "torrent-start");
}

void Torrent::startNow()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    session::sendRequest(request, "torrent-start-now");
}

void Torrent::stop()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    session::sendRequest(request, "torrent-stop");
}

void Torrent::verify()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    session::sendRequest(request, "torrent-verify");
}

void Torrent::askForMorePeers()
{
    nlohmann::json request;
    request["ids"] = { this->id() };
    session::sendRequest(request, "torrent-reannounce");
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
    session::sendRequest(request, "torrent-set");
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

    session::sendRequest(jsonFormat.output(), "torrent-set-location");
}

std::uint64_t Torrent::bytesUploaded() const
{
    return priv_->get_uploadedEver();
}
