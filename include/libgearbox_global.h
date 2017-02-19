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

#ifndef LIBGEARBOX_GLOBAL_H
#define LIBGEARBOX_GLOBAL_H

#if defined(_MSC_VER)
    /* Microsoft */
#   define EXPORT_SYMBOL __declspec(dllexport)
#   define IMPORT_SYMBOL __declspec(dllimport)
#elif defined(__GNUC__)
    /* GCC */
#   define EXPORT_SYMBOL __attribute__((visibility("default")))
#   define IMPORT_SYMBOL
#else
#   define EXPORT_SYMBOL
#   define IMPORT_SYMBOL
#   pragma warning Unknown dynamic link import/export semantics.
#endif

#if defined(GEARBOX_LIB)
#   define GEARBOX_API    EXPORT_SYMBOL
#else
#   define GEARBOX_API    IMPORT_SYMBOL
#endif

#define DISABLE_COPY(klass) \
    klass(const klass &other) = delete; \
    klass &operator =(const klass &other) = delete;

#define DISABLE_MOVE(klass) \
    klass(klass &&other) = delete; \
    klass &operator =(klass &&other) = delete;

#endif // LIBGEARBOX_GLOBAL_H
