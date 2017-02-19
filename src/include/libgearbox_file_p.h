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

#ifndef LIBGEARBOX_FILE_P_H
#define LIBGEARBOX_FILE_P_H

#include <sequential.h>

namespace gearbox
{
    struct FileAttr
    {
        ATTRIBUTE(std::uint64_t, bytesCompleted)
        ATTRIBUTE(std::uint64_t, length) /* size in bytes */
        ATTRIBUTE(std::string, name)
        INIT_ATTRIBUTES(bytesCompleted, length, name)
    };

    struct FileStat
    {
        ATTRIBUTE(std::uint64_t, bytesCompleted)
        ATTRIBUTE(bool, wanted) /* wether file should be downloaded or not */
        ATTRIBUTE(std::int8_t, priority)
        INIT_ATTRIBUTES(wanted, priority, bytesCompleted)
    };
}

#endif // LIBGEARBOX_FILE_P_H
