#include "libgearbox_folder_p.h"
#include "libgearbox_folder.h"
#include "libgearbox_vla_p.h"

#include <vector>
#include <cstring>

using namespace librt;

namespace
{
    inline char *find_char(char *string, const std::size_t size, char wanted)
    {
        char *c = nullptr;
        if (size > 0)
        {
            for (int it = static_cast<int>(size) - 1; it >= 0; --it)
            {
                if (string[it] == wanted)
                {
                    c = &string[it];
                    break;
                }
            }
        }
        return c;
    }

    std::vector<std::string> split(const std::string &path, char separator)
    {
        const auto len = path.size();
        auto fullyQualifiedName = make_vla(char, len + 1);
        std::strncpy(fullyQualifiedName, path.c_str(), len);
        auto separatorPtr = find_char(fullyQualifiedName, len, separator);

        std::vector<std::string> folders;
        while (separatorPtr > fullyQualifiedName)
        {
            std::size_t indexOfSeparator = static_cast<std::size_t>(separatorPtr - fullyQualifiedName);
            separatorPtr[0] = '\0';
            folders.push_back(&fullyQualifiedName[indexOfSeparator + 1]);
            separatorPtr = find_char(fullyQualifiedName, indexOfSeparator, '/');
        }

        return folders;
    }
}

FolderPrivate::FolderPrivate(std::string &&name) :
    name_(std::move(name))
{}

const std::string &FolderPrivate::name() const
{
    return name_;
}

const FolderPrivate::folder_array_t &FolderPrivate::subfolders() const
{
    return subfolders_;
}

const FolderPrivate::file_array_t &FolderPrivate::files() const
{
    return files_;
}

Folder *FolderPrivate::find(const std::string &name)
{
    Folder *f = nullptr;
    auto node = subfolders_.find(name);
    if (node != subfolders_.end())
        f = (node->second).get();
    return f;
}

Folder *FolderPrivate::insert(std::string folder)
{
    Folder *f = nullptr;
    std::unique_ptr<Folder> value(new Folder(std::move(folder)));
    auto pair = subfolders_.insert(std::make_pair(value->name(), std::move(value)));

    if (pair.second)
        f = pair.first->second.get();
    else
        throw std::runtime_error("Failed to add folder to unordered_map. Are you sure that the node does not already exist?");

    return f;
}

File *FolderPrivate::insert(File &&file)
{
    File *f = nullptr;
    auto value = std::make_unique<File>(std::move(file));
    auto pair = files_.insert(std::make_pair(value->name(), std::move(value)));

    if (pair.second)
        f = pair.first->second.get();

    return f;
}

void FolderPrivate::addPath(Folder &root, const std::string &path, std::size_t id,
                            std::uint64_t bytesCompleted, std::uint64_t length,
                            bool wanted, File::Priority priority)
{
    auto names = split(path, '/');
    Folder *node = &root;
    auto lastFolderIndex = names.empty() ? 0 : static_cast<int>(names.size()) - 1;
    for (int it = lastFolderIndex; it >= 1; --it)
    {
        auto newNode = node->priv_->find(names.at(static_cast<std::size_t>(it)));
        if (newNode == nullptr)
            newNode = node->priv_->insert(std::move(names.at(static_cast<std::size_t>(it))));
        node = newNode;
    }
    File *f = node->priv_->insert(File(std::move(names.at(0)), bytesCompleted, length, wanted, priority));
    f->id_ = id;
}

Folder::Folder(std::string &&name) :
    priv_(new FolderPrivate(std::forward<std::string>(name)))
{
}

Folder::Folder(Folder &&) noexcept(true) = default;
Folder &Folder::operator=(Folder &&) noexcept(true) = default;
Folder::~Folder() noexcept(true) = default;

Folder::Folder() :
    priv_(nullptr)
{
}

const std::string &Folder::name() const
{
    return priv_->name();
}

const std::vector<std::reference_wrapper<const Folder>> Folder::subfolders() const
{
    std::vector<std::reference_wrapper<const Folder>> subfolders;
    subfolders.reserve(priv_->subfolders().size());
    auto subfoldersEnd = priv_->subfolders().cend();
    for (auto it = priv_->subfolders().cbegin(); it != subfoldersEnd; ++it)
    {
        subfolders.push_back(std::cref(*(it->second.get())));
    }
    return subfolders;
}

const std::vector<std::reference_wrapper<const File>> Folder::files() const
{
    std::vector<std::reference_wrapper<const File>> files;
    files.reserve(priv_->files().size());
    auto filesEnd = priv_->files().cend();
    for (auto it = priv_->files().cbegin(); it != filesEnd; ++it)
    {
        files.push_back(std::cref(*(it->second.get())));
    }
    return files;
}

void Folder::addPath(Folder &root, const std::string &path, std::size_t id,
                     std::uint64_t bytesCompleted, std::uint64_t length,
                     bool wanted, File::Priority priority)
{
    root.priv_->addPath(root, path, id, bytesCompleted, length, wanted, priority);
}
