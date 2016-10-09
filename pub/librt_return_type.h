#ifndef LIBRT_RETURN_TYPE_H
#define LIBRT_RETURN_TYPE_H

#include <librt_global.h>
#include <librt_error.h>

namespace librt
{
    template <typename T>
    struct ReturnType
    {
        ReturnType(Error &&error, T &&value) :
            error(std::move(error)),
            value(std::move(value))
        {}

        ReturnType(ReturnType &&other)
        {
            error = std::move(other.error);
            value = std::move(other.value);
        }

        ReturnType &operator =(ReturnType &&other) noexcept(true)
        {
            error = std::move(other.error);
            value = std::move(other.value);
        }

        ReturnType(const ReturnType &other) = delete;
        ReturnType &operator =(const ReturnType &other) = delete;
        ~ReturnType() noexcept(true) = default;

        Error error;
        T value;
        constexpr operator T() { return std::move(value); }
    };
}

#endif // LIBRT_RETURN_TYPE_H
