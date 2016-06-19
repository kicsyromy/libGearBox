#ifndef LIBRT_RETURN_TYPE_H
#define LIBRT_RETURN_TYPE_H

#include <librt_global.h>
#include <librt_error.h>

namespace librt
{
    template <typename T>
    struct LIBRT_API ReturnType
    {
        ReturnType(Error &&error, T &&value) :
            error(std::move(error)),
            value(std::move(value))
        {}
        Error error;
        T value;
        inline operator T() { return std::move(value); }
    };
}

#endif // LIBRT_RETURN_TYPE_H
