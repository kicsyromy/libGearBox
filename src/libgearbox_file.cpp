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

#include "libgearbox_file.h"

#include <map>

#include "libgearbox_common_p.h"

using namespace gearbox;
using namespace gearbox::common;

namespace
{
    #include "libgearbox_mimetypes.h"

    inline const char *rfind_char(const char *string, const std::size_t size, char wanted)
    {
        const char *c = nullptr;
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

    File::MIMEType mimeType(const std::string &name)
    {
        const char *fileName = name.c_str();
        auto extension = rfind_char(fileName, name.size(), '.');
        if (extension != nullptr)
        {
            auto it = mimeTypes.find(extension);
            if (it != mimeTypes.end())
            {
                return it->second;
            }
            else
            {
                return { -1, "" };
            }
        }
        else
        {
            return { -1, "" };
        }
    }
}

File::File(std::string &&name,
           std::uint64_t bytesCompleted,
           std::uint64_t bytesTotal,
           bool wanted,
           File::Priority priority) :
    id_(0),
    name_(std::move(name)),
    bytesCompleted_(bytesCompleted),
    bytesTotal_(bytesTotal),
    wanted_(wanted),
    priority_(priority),
    type_(mimeType(name_))
{
}

const std::string &File::name() const
{
    return name_;
}

uint64_t File::bytesCompleted() const
{
    return bytesCompleted_;
}

uint64_t File::bytesTotal() const
{
    return bytesTotal_;
}

bool File::wanted() const
{
    return wanted_;
}

File::Priority File::priority() const
{
    return priority_;
}

File::MIMEType File::type() const
{
    return type_;
}
