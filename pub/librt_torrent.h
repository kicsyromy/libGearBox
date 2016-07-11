#ifndef LIBRT_TORRENT_H
#define LIBRT_TORRENT_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

#include <librt_global.h>
#include <librt_return_type.h>

namespace librt
{
    class TorrentPrivate;

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

        class File
        {
        public:
            enum class Priority
            {
                High,
                Normal,
                Low
            };

        public:
            File(File &&other);
            File &operator =(File &&other);

        public:
            const std::string &name() const;
            std::uint64_t bytesCompleted() const;
            std::uint64_t bytesTotal() const;
            bool wanted() const;
            Priority priority() const;

        private:
            File(const std::string &name,
                 std::uint64_t bytesCompleted,
                 std::uint64_t bytesTotal,
                 bool wanted,
                 Priority priority);

        private:
            std::size_t id_;
            std::string name_;
            std::uint64_t bytesCompleted_;
            std::uint64_t bytesTotal_;
            bool wanted_;
            Priority priority_;

            friend class librt::Torrent;
        };

    public:
        explicit Torrent(TorrentPrivate *priv);

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
        ReturnType<std::vector<File>> files() const;

        std::int32_t queuePosition() const;
        Error setQueuePosition(std::int32_t position);

        std::string downloadDir() const;
        Error setDownloadDir(const std::string &path, MoveType move = MoveType::SearchForExistingFiles);

    private:
        std::shared_ptr<TorrentPrivate> priv_;

    private:
        friend class Session;
    };
}

#endif // LIBRT_TORRENT_H
