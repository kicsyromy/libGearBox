#ifndef LIBRT_LOGGER_P_H
#define LIBRT_LOGGER_P_H

#include <fmt/format.h>

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#define LOG(...) \
    do { \
    std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "librt: INFO: {}: {}: {}:\n{}\n", \
            &__FILE__[strrchr(__FILE__, '/') - __FILE__ + 1], \
            __PRETTY_FUNCTION__, \
            __LINE__, \
            log); \
    } while (0)

#define LOG_INFO(message) LOG(message)

#define LOG_WARN(...) \
    do { \
    std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "librt: WARNING: {}: {}: {}:\n{}\n", \
            &__FILE__[strrchr(__FILE__, '/') - __FILE__ + 1], \
            __PRETTY_FUNCTION__, \
            __LINE__, \
            log); \
    } while (0)

#define LOG_ERROR(...) \
    do { \
    std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "librt: ERROR: {}: {}: {}:\n{}\n", \
            &__FILE__[strrchr(__FILE__, '/') - __FILE__ + 1], \
            __PRETTY_FUNCTION__, \
            __LINE__, \
            log); \
    } while (0)

#define LOG_FATAL(...) \
    do { \
    std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "librt: FATAL: {}: {}: {}:\n{}\n", \
            &__FILE__[strrchr(__FILE__, '/') - __FILE__ + 1], \
            __PRETTY_FUNCTION__, \
            __LINE__, \
            log); \
    std::terminate(); \
    } while (0)

#ifdef DEBUG_BUILD
#define LOG_DEBUG(...) \
    do { \
    std::string log = fmt::format(__VA_ARGS__); \
    fmt::print(stderr, \
            "librt: DEBUG: {}: {}: {}:\n{}\n", \
            &__FILE__[strrchr(__FILE__, '/') - __FILE__ + 1], \
            __PRETTY_FUNCTION__, \
            __LINE__, \
            log); \
    } while (0)
#else
#define LOG_DEBUG(...)
#endif

#endif // LIBRT_LOGGER_P_H
