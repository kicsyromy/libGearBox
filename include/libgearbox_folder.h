#ifndef LIBGEARBOX_FOLDER_H
#define LIBGEARBOX_FOLDER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <libgearbox_global.h>
#include <libgearbox_file.h>

namespace gearbox
{
    class Torrent;
    class FolderPrivate;

    class GEARBOX_API Folder
    {
    public:
        Folder(); // make gearbox::ReturnType happy
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

#endif // LIBGEARBOX_FOLDER_H