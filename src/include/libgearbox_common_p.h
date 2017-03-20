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

#ifndef LIBGEARBOX_COMMON_P_H
#define LIBGEARBOX_COMMON_P_H

#include <string>
#include <type_traits>

/* For whatever reason clang refuses to compile this code */
#ifndef __clang__
#define TYPE_HAS_METHOD(type_name, method_name, signature)                        \
    template<typename T>                                                          \
    struct has_##method_name {                                                    \
        static_assert(std::integral_constant<T, false>::value, "");               \
    };                                                                            \
    template <typename Ret, typename ...Args>                                     \
    struct has_##method_name<Ret(Args...)> {                                      \
    private:                                                                      \
        template<typename T>                                                      \
        static constexpr auto check(T*)                                           \
        -> typename                                                               \
            std::is_same<                                                         \
                decltype(std::declval<T>().method_name(std::declval<Args>()...)), \
                Ret                                                               \
            >::type;                                                              \
        template<typename> static constexpr std::false_type check(...);           \
        typedef decltype(check<typename type_name>(0)) type;                      \
    public: static constexpr bool value = type::value; };                         \
    static constexpr const bool has_##method_name##_v = has_##method_name<signature>::value
#else
#define TYPE_HAS_METHOD(type_name, method_name, signature) \
    static constexpr const bool has_##method_name##_v = true
#endif

namespace gearbox
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

#endif // LIBGEARBOX_COMMON_P_H
