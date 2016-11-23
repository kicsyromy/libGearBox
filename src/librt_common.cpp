#include "librt_common_p.h"

#include <algorithm>
#include <cstring>

using namespace librt::common;

bool CaseInsensitiveCompare::operator()(const std::string &s1, const std::string &s2) const
{
    return std::lexicographical_compare(
        s1.begin(), s1.end(), s2.begin(), s2.end(),
        [](const auto &c1, const auto &c2) { return std::tolower(c1) < std::tolower(c2); });
}

bool CaseInsensitiveCompare::operator()(const char *s1, const char *s2) const
{
    const char *ap = s1;
    const char *bp = s2;
    bool result = false;

    while ((*ap != '\0') && (*bp != '\0'))
    {
        if (std::tolower(*ap) < std::tolower(*bp))
        {
            result = true;
            break;
        }
        if (std::tolower(*ap++) > std::tolower(*bp++))
        {
            result = false;
            break;
        }
    }

    if (*ap == *bp)
    {
        result = false;
    }

    if ((*ap == '\0') && (*bp != '\0'))
    {
        result = true;
    }

    return result;
}
