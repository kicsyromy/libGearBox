#ifndef LIBGEARBOX_RETURN_TYPE_H
#define LIBGEARBOX_RETURN_TYPE_H

#include <libgearbox_global.h>
#include <libgearbox_error.h>

namespace gearbox
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

        ~ReturnType() noexcept(true) = default;

        Error error;
        T value;
        constexpr operator T() { return std::move(value); }

    private:
        DISABLE_COPY(ReturnType)
    };
}

#endif // LIBGEARBOX_RETURN_TYPE_H
