#ifndef LIBRT_VLA_P_H
#define LIBRT_VLA_P_H

#include <cstddef>
#include <malloc.h>

#define make_vla_impl(type, size) new(static_cast<type*>(alloca(sizeof(type) * size))) type[size], size
#define make_vla(type, size) ::librt::Array<type>(make_vla_impl(type, size)) 

namespace librt
{
    template <typename T>
    class Array
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
        Array &operator =(const Array &) = delete;

        Array(Array &&) noexcept(true) = default;
        Array &operator =(Array &&) noexcept(true) = default;

    public:
        inline operator T *() { return array_; }
        inline std::size_t size() { return size_; }

    private:
        T *array_;
        std::size_t size_;
    };
}

#endif // LIBRT_VLA_P_H
