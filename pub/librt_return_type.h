#include "librt_error.h"

#include <librt_global.h>

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
