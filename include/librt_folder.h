#ifndef LIBRT_FOLDER_H
#define LIBRT_FOLDER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <librt_global.h>
#include <librt_file.h>

namespace librt
{
    class Torrent;
    class FolderPrivate;

    class LIBRT_API Folder
    {
    public:
        Folder(); // make librt::ReturnType happy
        Folder(Folder &&) noexcept(true);
        Folder &operator =(Folder &&) noexcept(true);
        ~Folder() noexcept(true);

    public:
        const std::string &name() const;
        const std::vector<std::reference_wrapper<const Folder>> subfolders() const;
        const std::vector<std::reference_wrapper<const File>> files() const;

    private:
        Folder(std::string &&name);

    private:
        static void addPath(Folder &root, const std::string &path, std::size_t id,
                            std::uint64_t bytesCompleted, std::uint64_t length,
                            bool wanted, File::Priority priority);

    private:
        std::unique_ptr<FolderPrivate> priv_;

    private:
        friend class FolderPrivate;
        friend class Torrent;

    private:
        DISABLE_COPY(Folder)
    };
}

#endif // LIBRT_FOLDER_H
