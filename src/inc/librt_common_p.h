#ifndef LIBRT_COMMON_P_H
#define LIBRT_COMMON_P_H

#include <string>

namespace librt
{
    namespace common
    {
        struct CaseInsensitiveCompare
        {
            bool operator()(const std::string &s1, const std::string &s2) const;
            bool operator()(const char *s1, const char *s2) const;
        };
    }
}

#endif // LIBRT_COMMON_P_H
