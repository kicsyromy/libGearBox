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

#ifndef LIBGEARBOX_VLA_P_H
#define LIBGEARBOX_VLA_P_H

#include <cstddef>

#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_UWP)
#include <malloc.h>
#else
#include <alloca.h>
#endif

#define make_vla_impl(type, size)                                              \
    new (static_cast<type *>(alloca(sizeof(type) * size))) type[size], size
#define make_vla(type, size) ::gearbox::Array<type>(make_vla_impl(type, size))

namespace gearbox
{
    template <typename T> class Array
    {
    public:
        Array() : array_(nullptr), size_(0) {}
        Array(T *handle, std::size_t size) : array_(handle), size_(size) {}
        ~Array()
        {
            for (std::size_t i = 0; i < size_; ++i)
            {
                array_[i].~T();
            }
        }

        Array(const Array &) = delete;
        Array &operator=(const Array &) = delete;

        Array(Array &&) noexcept(true) = default;
        Array &operator=(Array &&) noexcept(true) = default;

    public:
        inline operator T *() { return array_; }
        inline std::size_t size() { return size_; }

    private:
        T *array_;
        std::size_t size_;
    };
}

#endif // LIBGEARBOX_VLA_P_H
