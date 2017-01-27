#ifndef FOLDER_P_H
#define FOLDER_P_H

#include <unordered_map>
#include <memory>
#include <string>

#include <libgearbox_file.h>

namespace librt
{
    class Folder;

    class FolderPrivate
    {
    public:
        using folder_array_t = std::unordered_map<std::string, std::unique_ptr<Folder>>;
        using file_array_t   = std::unordered_map<std::string, std::unique_ptr<File>>;

    public:
        FolderPrivate(std::string &&name);

    public:
        const std::string &name() const;

    public:
        const folder_array_t &subfolders() const;
        const file_array_t &files() const;

    public:
        Folder *find(const std::string &name);
        Folder *insert(std::string folder);
        File *insert(File &&file);

    public:
        static void addPath(Folder &root, const std::string &path, std::size_t id,
                            std::uint64_t bytesCompleted, std::uint64_t length,
                            bool wanted, File::Priority priority);

    private:
        std::string name_;
        folder_array_t subfolders_;
        file_array_t files_;
    };
}

#endif // FOLDER_P_H
