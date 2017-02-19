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

#ifndef LIBGEARBOX_FOLDER_P_H
#define LIBGEARBOX_FOLDER_P_H

#include <unordered_map>
#include <memory>
#include <string>

#include <libgearbox_file.h>

namespace gearbox
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

#endif // LIBGEARBOX_FOLDER_P_H
