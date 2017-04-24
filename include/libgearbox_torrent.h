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

#ifndef LIBGEARBOX_TORRENT_H
#define LIBGEARBOX_TORRENT_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <libgearbox_file.h>
#include <libgearbox_folder.h>
#include <libgearbox_global.h>
#include <libgearbox_return_type.h>

namespace gearbox
{
    class TorrentPrivate;
    class FolderPrivate;

    class GEARBOX_API Torrent
    {
    public:
        enum class Status
        {
            Stopped = 0,
            CheckWait = 1,
            Check = 2,
            DownloadWait = 3,
            Download = 4,
            SeedWait = 5,
            Seed = 6,
            Invalid = 255
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
        bool operator==(const Torrent &other) const;
        bool operator<(const Torrent &other) const;

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
        Error setWantedFiles(
            const std::vector<std::reference_wrapper<const File>> &files);
        Error setSkippedFiles(
            const std::vector<std::reference_wrapper<const File>> &files);

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
        Error setDownloadDir(const std::string &path,
                             MoveType move = MoveType::SearchForExistingFiles);

    private:
        std::unique_ptr<TorrentPrivate> priv_;

    private:
        friend class Session;

    private:
        DISABLE_COPY(Torrent)
    };
}

#endif // LIBGEARBOX_TORRENT_H
