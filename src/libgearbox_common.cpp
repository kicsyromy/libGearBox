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

#include "libgearbox_common_p.h"

#include <algorithm>
#include <cstring>
#include <cctype>

#ifndef PLATFORM_WINDOWS
#include <strings.h>
#endif

using namespace gearbox::common;

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
