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

/*!
    \class gearbox::Folder
    \brief A folder from a torrent's file hierarchy.

    It's represented by a name and consists of a set of folders and files.
    Folders are created solely by the gearbox::Torrent class by calling
    gearbox::Torrent::content() const on a valid instance.
*/

#include "libgearbox_folder_p.h"
#include "libgearbox_folder.h"
#include "libgearbox_vla_p.h"

#include <vector>
#include <cstring>

#ifdef _MSC_VER
/* Disable warning about strncpy not being safe */
#pragma warning(disable: 4996)
#endif // _MSC_VER

using namespace gearbox;

namespace
{
    /* Perform a search in 'string', from the end, and return the address of the last character */
    /* that matches 'wanted'                                                                    */      
    inline char *rfind_char(char *string, const std::size_t size, char wanted)
    {
        char *c = nullptr;
        if (size > 0)
        {
            for (std::size_t it = size - 1;; --it)
            {
                if (string[it] == wanted)
                {
                    c = &string[it];
                    break;
                }

                if (it == 0)
                {
                    break;
                }
            }
        }
        return c;
    }

    /* Splits a string into a vector based on 'separator'. The elements are stored in reverse */
    /* order, with the first element of the string, last element of the vector, ignored       */
    std::vector<std::string> rsplit(const std::string &path, char separator)
    {
        const auto len = path.size();
        auto fullyQualifiedName = make_vla(char, len + 1);
        std::strncpy(fullyQualifiedName, path.c_str(), len);
        fullyQualifiedName[len] = '\0';
        auto separatorPtr = rfind_char(fullyQualifiedName, len, separator);

        std::vector<std::string> folders;
        while (separatorPtr > fullyQualifiedName)
        {
            std::size_t indexOfSeparator = static_cast<std::size_t>(separatorPtr - fullyQualifiedName);
            separatorPtr[0] = '\0';
            folders.push_back(&fullyQualifiedName[indexOfSeparator + 1]);
            separatorPtr = rfind_char(fullyQualifiedName, indexOfSeparator, separator);
        }

        return folders;
    }
}


FolderPrivate::FolderPrivate(std::string &&name, Folder &folder) :
    folder_(folder),
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

void FolderPrivate::addPath(const std::string &path, std::size_t id,
                            std::uint64_t bytesCompleted, std::uint64_t length,
                            bool wanted, File::Priority priority)
{
    auto names = rsplit(path, '/');
    Folder *node = &folder_;
    std::size_t lastFolderIndex = names.empty() ? 0 : names.size() - 1;
    for (std::size_t it = lastFolderIndex; it >= 1; --it)
    {
        auto newNode = node->priv_->find(names.at(it));
        if (newNode == nullptr)
            newNode = node->priv_->insert(std::move(names.at(it)));
        node = newNode;
    }
    File *f = node->priv_->insert(File(std::move(names.at(0)), bytesCompleted, length, wanted, priority));
    f->id_ = id;
}

Folder::Folder(std::string &&name) :
    priv_(new FolderPrivate(std::forward<std::string>(name), *this))
{
}

Folder::Folder(Folder &&) noexcept(true) = default;
Folder &Folder::operator=(Folder &&) noexcept(true) = default;
Folder::~Folder() noexcept(true) = default;

Folder::Folder() :
    priv_(nullptr)
{
}

/*!
    Returns the \c name of the folder.
*/
const std::string &Folder::name() const
{
    return priv_->name();
}

/*!
    Returns a list of subfolders from the current folder.

    Subfolders are valid as long as the folder on which this function was called is valid.
*/
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

/*!
    Returns a list of files from the current folder.

    Files are valid as long as the folder on which this function was called is valid.
*/
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
