#include "librt_error.h"

namespace librt
{
    template <typename T>
    struct ReturnType
    {
        ReturnType(Error &&error, T &&value) :
            error(std::move(error)),
            value(std::move(value))
        {}
        Error error;
        T value;
        inline operator T &() { return value; }
    };
}
