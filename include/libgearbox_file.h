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

#ifndef LIBGEARBOX_FILE_H
#define LIBGEARBOX_FILE_H

#include <string>

#include <libgearbox_global.h>

namespace gearbox
{
    class Torrent;
    class FolderPrivate;

    class GEARBOX_API File
    {
    public:
        enum class Priority
        {
            High,
            Normal,
            Low
        };

        struct MIMEType
        {
            int id;
            const char *const name;
            inline bool operator==(const MIMEType &other) const
            {
                return id == other.id;
            }
            inline bool isValid() const { return id >= 0; }
        };

    public:
        File(File &&) = default;
        ~File() = default;
        File &operator=(File &&) = default;

    public:
        const std::string &name() const;
        std::uint64_t bytesCompleted() const;
        std::uint64_t bytesTotal() const;
        bool wanted() const;
        Priority priority() const;

    public:
        MIMEType type() const;

    private:
        File(std::string &&name,
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
        MIMEType type_;

    private:
        friend class Torrent;
        friend class FolderPrivate;

    private:
        DISABLE_COPY(File)
    };
}

#endif // LIBGEARBOX_FILE_H
