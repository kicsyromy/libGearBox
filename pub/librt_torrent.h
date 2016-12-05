#ifndef LIBRT_TORRENT_H
#define LIBRT_TORRENT_H

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <memory>

#include <librt_file.h>
#include <librt_folder.h>
#include <librt_global.h>
#include <librt_return_type.h>

namespace librt
{
    class TorrentPrivate;
    class FolderPrivate;

    class LIBRT_API Torrent
    {
    public:
        enum class Status
        {
            Stopped        = 0,
            CheckWait      = 1,
            Check          = 2,
            DownloadWait   = 3,
            Download       = 4,
            SeedWait       = 5,
            Seed           = 6,
            Invalid        = 255
        };

        enum class MoveType
        {
            MoveToNewLocation,
            SearchForExistingFiles
        };

        enum class LocalDataAction
        {
            KeepFiles,
            DeleteFiles
        };

    public:
        explicit Torrent(TorrentPrivate *priv);
        Torrent(Torrent &&);
        Torrent &operator=(Torrent &&) noexcept(true);
        ~Torrent() noexcept(true);

    public:
        bool operator ==(const Torrent &other) const;
        bool operator  <(const Torrent &other) const;

    public:
        bool valid() const;

    public:
        Error start();
        Error startNow();
        Error stop();
        Error verify();
        Error askForMorePeers();
        Error remove(LocalDataAction action = LocalDataAction::KeepFiles);
        Error queueMoveUp();
        Error queueMoveDown();
        Error queueMoveTop();
        Error queueMoveBottom();
        Error update();
        Error setWantedFiles(const std::vector<std::reference_wrapper<const File>> &files);
        Error setSkippedFiles(const std::vector<std::reference_wrapper<const File>> &files);

    public:
        std::int32_t id() const;
        std::string name() const;
        std::uint64_t bytesDownloaded() const;
        double percentDone() const;
        double uploadRatio() const;
        std::uint64_t bytesUploaded() const;
        std::uint64_t downloadSpeed() const;
        std::uint64_t uploadSpeed() const;
        Status status() const;
        std::uint64_t size() const;
        std::int32_t eta() const;
        ReturnType<Folder> content() const;
        ReturnType<std::vector<File>> files() const;

        std::int32_t queuePosition() const;
        Error setQueuePosition(std::int32_t position);

        std::string downloadDir() const;
        Error setDownloadDir(const std::string &path, MoveType move = MoveType::SearchForExistingFiles);

    private:
        std::unique_ptr<TorrentPrivate> priv_;

    private:
        friend class Session;

    private:
        DISABLE_COPY(Torrent)
    };
}

#endif // LIBRT_TORRENT_H
