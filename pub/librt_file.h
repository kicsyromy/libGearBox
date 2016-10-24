#ifndef LIBRT_FILE_H
#define LIBRT_FILE_H

#include <string>

#include <librt_global.h>

namespace librt
{
    class Torrent;
    class FolderPrivate;

    class LIBRT_API File
    {
    public:
        enum class Priority
        {
            High,
            Normal,
            Low
        };

        enum class MIMEType
        {
            Unknown
        };

    public:
        File(File &&) = default;
        ~File() = default;
        File &operator =(File &&) = default;

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
             Priority priority,
             MIMEType type);
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

#endif // LIBRT_FILE_H
