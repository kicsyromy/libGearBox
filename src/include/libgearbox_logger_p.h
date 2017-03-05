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

#ifndef LIBGEARBOX_LOGGER_P_H
#define LIBGEARBOX_LOGGER_P_H

#include <fmt/format.h>

#ifdef _MSC_VER
    #define PATH_SEPARATOR '\\'
#else
    #define PATH_SEPARATOR '/'
#endif // _MSC_VER

#define LOG(...) \
    do { \
    const std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "libgearbox: INFO: {}: {}: {}:\n{}\n", \
            strrchr(__FILE__, PATH_SEPARATOR) + 1, \
            __LINE__, \
            __func__, \
            log); \
    } while (0)

#define LOG_INFO(message) LOG(message)

#define LOG_WARN(...) \
    do { \
    const std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "libgearbox: WARNING: {}: {}: {}:\n{}\n", \
            strrchr(__FILE__, PATH_SEPARATOR) + 1, \
            __LINE__, \
            __func__, \
            log); \
    } while (0)

#define LOG_ERROR(...) \
    do { \
    const std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "libgearbox: ERROR: {}: {}: {}:\n{}\n", \
            strrchr(__FILE__, PATH_SEPARATOR) + 1, \
            __LINE__, \
            __func__, \
            log); \
    } while (0)

#define LOG_FATAL(...) \
    do { \
    const std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "libgearbox: FATAL: {}: {}: {}:\n{}\n", \
            strrchr(__FILE__, PATH_SEPARATOR) + 1, \
            __LINE__, \
            __func__, \
            log); \
    std::terminate(); \
    } while (0)

#ifdef GEARBOX_LOG_DEBUG
#define LOG_DEBUG(...) \
    do { \
    const std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "libgearbox: DEBUG: {}: {}: {}:\n{}\n", \
            strrchr(__FILE__, PATH_SEPARATOR) + 1, \
            __LINE__, \
            __func__, \
            log); \
    } while (0)
#else
#define LOG_DEBUG(...)
#endif // GEARBOX_LOG_DEBUG

#endif // LIBGEARBOX_LOGGER_P_H
