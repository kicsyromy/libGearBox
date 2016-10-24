#ifndef LIBRT_TORRENT_P_H
#define LIBRT_TORRENT_P_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

#include <sequential.h>

#include "librt_file_p.h"
#include "librt_session_p.h"

namespace librt
{
    class TorrentPrivate
    {
        ATTRIBUTE(std::int32_t, id)
        ATTRIBUTE(std::string, name)
        ATTRIBUTE(std::uint64_t, haveValid) /* bytes downloaded */
        ATTRIBUTE(double, percentDone)
        ATTRIBUTE(double, uploadRatio)
        ATTRIBUTE(std::uint64_t, uploadedEver) /* bytes uploaded */
        ATTRIBUTE(std::uint64_t, rateDownload)
        ATTRIBUTE(std::uint64_t, rateUpload)
        ATTRIBUTE(std::int32_t, status)
        ATTRIBUTE(std::uint64_t, totalSize)
        ATTRIBUTE(std::string, downloadDir)
        ATTRIBUTE(std::int32_t, eta)
        ATTRIBUTE(std::int32_t, queuePosition)
        INIT_ATTRIBUTES(
                id,
                name,
                haveValid,
                percentDone,
                uploadRatio,
                uploadedEver,
                rateDownload,
                rateUpload,
                status,
                totalSize,
                downloadDir,
                eta,
                queuePosition
        )
    public:
        struct Files
        {
            ATTRIBUTE(std::vector<FileAttr>, files)
            ATTRIBUTE(std::vector<FileStat>, fileStats)
            INIT_ATTRIBUTES(files, fileStats)

            struct Request
            {
                ATTRIBUTE(std::vector<const char *>, fields)
                ATTRIBUTE(std::vector<std::int32_t>, ids)
                INIT_ATTRIBUTES(fields, ids)

                Request()
                {
                    sequential::attribute::set_value<Request::fields>(
                        *this, std::move(::librt::TorrentPrivate::Files::attribute_names())
                    );
                }
            };

            struct Response
            {
                ATTRIBUTE(std::vector<TorrentPrivate::Files>, torrents)
                INIT_ATTRIBUTES(torrents)
            };
        };

    public:
        struct Response
        {
            ATTRIBUTE(std::vector<TorrentPrivate>, torrents)
            ATTRIBUTE(std::vector<std::int32_t>, removed)
            INIT_ATTRIBUTES(torrents, removed)
        };

        struct MoveRequest
        {
            ATTRIBUTE(std::vector<std::int32_t>, ids)
            ATTRIBUTE(std::string, location)
            ATTRIBUTE(bool, move)
            INIT_ATTRIBUTES(ids, location, move)
        };

    public:
        TorrentPrivate();
        TorrentPrivate(TorrentPrivate &&other);
        TorrentPrivate &operator =(TorrentPrivate &&other);
        TorrentPrivate(const TorrentPrivate &other);
        TorrentPrivate &operator =(const TorrentPrivate &other);

    public:
        std::weak_ptr<SessionPrivate> session_;
    };
}

#endif // LIBRT_TORRENT_P_H
