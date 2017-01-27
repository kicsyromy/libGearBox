#ifndef LIBRT_LOGGER_P_H
#define LIBRT_LOGGER_P_H

#include <fmt/format.h>

#ifdef PLATFORM_WINDOWS
    #ifdef _MSC_VER
        #define __PRETTY_FUNCTION__ __FUNCTION__
    #endif // _MSC_VER
    #define PATH_SEPARATOR '\\'
#else
    #define PATH_SEPARATOR '/'
#endif // PLATFORM_WINDOWS

#define LOG(...) \
    do { \
    std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "librt: INFO: {}: {}: {}:\n{}\n", \
            &__FILE__[strrchr(__FILE__, PATH_SEPARATOR) - __FILE__ + 1], \
            __PRETTY_FUNCTION__, \
            __LINE__, \
            log); \
    } while (0)

#define LOG_INFO(message) LOG(message)

#define LOG_WARN(...) \
    do { \
    std::string log = fmt::format(__VA_ARGS__)); \
    fmt::print(stderr, \
            "librt: WARNING: {}: {}: {}:\n{}\n", \
            &__FILE__[strrchr(__FILE__, PATH_SEPARATOR) - __FILE__ + 1], \
            __PRETTY_FUNCTION__, \
            __LINE__, \
            log); \
    } while (0)

#define LOG_ERROR(...) \
    do { \
    std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "librt: ERROR: {}: {}: {}:\n{}\n", \
            &__FILE__[strrchr(__FILE__, PATH_SEPARATOR) - __FILE__ + 1], \
            __PRETTY_FUNCTION__, \
            __LINE__, \
            log); \
    } while (0)

#define LOG_FATAL(...) \
    do { \
    std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "librt: FATAL: {}: {}: {}:\n{}\n", \
            &__FILE__[strrchr(__FILE__, PATH_SEPARATOR) - __FILE__ + 1], \
            __PRETTY_FUNCTION__, \
            __LINE__, \
            log); \
    std::terminate(); \
    } while (0)

#ifdef GEARBOX_LOG_DEBUG
#define LOG_DEBUG(...) \
    do { \
    std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "librt: DEBUG: {}: {}: {}:\n{}\n", \
            __PRETTY_FUNCTION__, \
            &__FILE__[strrchr(__FILE__, PATH_SEPARATOR) - __FILE__ + 1], \
            __LINE__, \
            log); \
    } while (0)
#else
#define LOG_DEBUG(...)
#endif // RT_LOG_DEBUG

#endif // LIBRT_LOGGER_P_H
