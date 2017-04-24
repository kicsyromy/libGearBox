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

#ifndef LIBGEARBOX_FOLDER_H
#define LIBGEARBOX_FOLDER_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <libgearbox_file.h>
#include <libgearbox_global.h>

namespace gearbox
{
    class Torrent;
    class FolderPrivate;

    class GEARBOX_API Folder
    {
    public:
        Folder(); /* make gearbox::ReturnType happy */
        Folder(Folder &&) noexcept(true);
        Folder &operator=(Folder &&) noexcept(true);
        ~Folder() noexcept(true);

    public:
        const std::string &name() const;
        const std::vector<std::reference_wrapper<const Folder>> subfolders()
            const;
        const std::vector<std::reference_wrapper<const File>> files() const;

    private:
        Folder(std::string &&name);

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
