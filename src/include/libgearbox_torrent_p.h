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

#ifndef LIBGEARBOX_TORRENT_P_H
#define LIBGEARBOX_TORRENT_P_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

#include <sequential.h>

#include "libgearbox_file_p.h"
#include "libgearbox_session_p.h"

namespace gearbox
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
                        *this, ::gearbox::TorrentPrivate::Files::attribute_names()
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

#endif // LIBGEARBOX_TORRENT_P_H
