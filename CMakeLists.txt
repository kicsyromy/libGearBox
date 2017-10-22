cmake_minimum_required (VERSION 3.0)
project (libgearbox CXX)

list (APPEND CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (ConfigureTarget)
include (Documentation)
include (ClangFormat)

set (LIBGEARBOX_VERSION_MAJOR 0)
set (LIBGEARBOX_VERSION_MINOR 1)
set (LIBGEARBOX_VERSION_PATCH 0)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED YES)

option (LIBGEARBOX_STATIC_LIB "Build static library if set to ON otherwise build shared library." ON)
option (LIBGEARBOX_DEBUG_LOGS "Build with additional debug logs. CAUTION: Do not use in production." OFF)
option (LIBGEARBOX_ENABLE_TESTS "Build tests." OFF)
option (LIBGEARBOX_GENERATE_DOCUMENTATION "Build documentation." OFF)
option (LIBGEARBOX_BUILD_BINDING_LAYER "Build binding helper library." OFF)

## PRIVATE HEADERS ##
file (GLOB_RECURSE LIBGEARBOX_PRIVATE_HEADERS "${PROJECT_SOURCE_DIR}/src/include/*.h")
source_group ("Header Files\\Private" FILES ${LIBGEARBOX_PRIVATE_HEADERS})

## PUBLIC HEADERS ##
file (GLOB_RECURSE LIBGEARBOX_PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")
source_group ("Header Files\\Public" FILES ${LIBGEARBOX_PUBLIC_HEADERS})

## SOURCES ##
file (GLOB_RECURSE LIBGEARBOX_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
source_group ("Source Files" FILES ${LIBGEARBOX_SOURCES})

libgearbox_configure_target (
    ${PROJECT_NAME} ${LIBGEARBOX_STATIC_LIB}
    "${LIBGEARBOX_PUBLIC_HEADERS}" "${LIBGEARBOX_PRIVATE_HEADERS}" "${LIBGEARBOX_SOURCES}"
    ${LIBGEARBOX_VERSION_MAJOR} ${LIBGEARBOX_VERSION_MINOR} ${LIBGEARBOX_VERSION_PATCH}
)

## PRIVATE HEADERS ##
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src/include")

## PUBLIC HEADERS ##
target_include_directories (${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories (${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}/config")

## NLOHMANN JSON ##
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/json/src")

## SEQUENTIAL SERIALIZER ##
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/sequential")

## C++ FORMAT ##
target_compile_definitions (${PROJECT_NAME} PRIVATE "-DFMT_HEADER_ONLY")
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/fmt")

if (LIBGEARBOX_DEBUG_LOGS)
    target_compile_definitions (${PROJECT_NAME} PRIVATE "-DLIBGEARBOX_LOG_DEBUG")
    message (STATUS "Building with debug informaton and logging")
endif ()

install (
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install (
    FILES "${LIBGEARBOX_PUBLIC_HEADERS}"
    DESTINATION include/gearbox
)

install (
    FILES "${PROJECT_BINARY_DIR}/config/libgearbox_config.h"
    DESTINATION include/gearbox
)

if (LIBGEARBOX_ENABLE_TESTS)
    add_subdirectory (tests)
endif ()

if (LIBGEARBOX_BUILD_BINDING_LAYER)
    add_subdirectory (bind)
endif ()
