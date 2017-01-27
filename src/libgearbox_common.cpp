#include "librt_common_p.h"

#include <algorithm>
#include <cstring>
#include <cctype>

#ifndef PLATFORM_WINDOWS
#include <strings.h>
#endif

using namespace librt::common;

bool CaseInsensitiveCompare::operator()(const std::string &s1, const std::string &s2) const
{
    return std::lexicographical_compare(
        s1.begin(), s1.end(), s2.begin(), s2.end(),
        [](const auto &c1, const auto &c2) { return std::tolower(c1) < std::tolower(c2); });
}

bool CaseInsensitiveCompare::operator()(const char *s1, const char *s2) const
{
#ifdef PLATFORM_WINDOWS
    return (_stricmp(s1, s2) < 0);
#else
    return (strcasecmp(s1, s2) < 0);
#endif
}
