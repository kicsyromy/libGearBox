cmake_minimum_required (VERSION 2.8.12)
project (librt)
add_definitions (-DRT_LIB)

set (librt_VERSION_MAJOR 0)
set (librt_VERSION_MINOR 0)
set (librt_VERSION_PATCH 3)
add_definitions (-DLIBRT_VERISION=${librt_VERSION_MAJOR}${LIBRT_VERSION_MINOR}${LIBRT_VERSION_PATCH})
add_definitions (-DLIBRT_VERISION_STR="${librt_VERSION_MAJOR}.${librt_VERSION_MINOR}.${librt_VERSION_PATCH}")

option (RT_STATIC_LIB "Build static library if set to ON otherwise build shared library." ON)
option (RT_DEBUG_LOGS "Build with additional debug logs. CAUTION: Do not use in production." OFF)
option (RT_ALLOW_UNSUPPORTED_COMPILER "Allow compilation using an unsupported compiler." OFF)
option (RT_ALLOW_UNSUPPORTED_PLATFORM "Allow compilation on an unsupported platform." OFF)

list (APPEND CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(LinuxConfig)
include(macOSConfig)
include(WindowsConfig)
include(TestCompilerSupport)

if (NOT RT_PLATFORM_OK)
    message (WARNING "Building on unsupported platform")
    if (NOT RT_ALLOW_UNSUPPORTED_PLATFORM)
        message (FATAL_ERROR "Stopping since RT_ALLOW_UNSUPPORTED_PLATFORM is not set")
    endif ()
endif ()

## PRIVATE HEADERS ##
include_directories ("${PROJECT_SOURCE_DIR}/src/inc")
set (librt_PRIVATE_HEADERS "${PROJECT_SOURCE_DIR}/src/inc")
file (GLOB_RECURSE PRIVATE_HEADERS "${librt_PRIVATE_HEADERS}/*.h")
add_custom_target (private_headers SOURCES ${PRIVATE_HEADERS})

## PUBLIC HEADERS ##
include_directories ("${PROJECT_SOURCE_DIR}/pub")
set (librt_PUBLIC_HEADERS  "${PROJECT_SOURCE_DIR}/pub")
file (GLOB_RECURSE PUBLIC_HEADERS "${librt_PUBLIC_HEADERS}/*.h")
add_custom_target (public_headers SOURCES ${PUBLIC_HEADERS})

## NLOHMANN JSON ##
include_directories ("${PROJECT_SOURCE_DIR}/3rdparty/json/src")
file (GLOB_RECURSE JSON_HEADERS "${PROJECT_SOURCE_DIR}/3rdparty/json/src/*.hpp")
add_custom_target (json_headers SOURCES ${JSON_HEADERS})

## SEQUENTIAL SERIALIZER ##
include_directories ("${PROJECT_SOURCE_DIR}/3rdparty/sequential")
file (GLOB_RECURSE SEQ_HEADERS "${PROJECT_SOURCE_DIR}/3rdparty/sequential/*.h")
add_custom_target (seq_headers SOURCES ${SEQ_HEADERS})

## C++ FORMAT ##
include_directories ("${PROJECT_SOURCE_DIR}/3rdparty/fmt")
set (FMT_SOURCE "${PROJECT_SOURCE_DIR}/3rdparty/fmt/fmt/format.cc")

include_directories (SYSTEM ${RT_INCLUDE_DIRS})
link_directories (${RT_LIB_DIRS})

aux_source_directory (${PROJECT_SOURCE_DIR}/src LIBRT_SOURCES)

if (RT_DEBUG_LOGS)
    add_definitions (-DRT_LOG_DEBUG)
    message (STATUS "Building with debug informaton and logging")
endif ()

if (RT_STATIC_LIB)
    add_library (rt STATIC ${LIBRT_SOURCES} ${FMT_SOURCE})
    message (STATUS "Building static library")
else ()
    add_library (rt SHARED ${LIBRT_SOURCES} ${FMT_SOURCE})
    message (STATUS "Building shared library")
endif ()

target_link_libraries (rt ${RT_LIBS})
set_target_properties (
    rt         PROPERTIES
    VERSION    ${librt_VERSION_MAJOR}.${librt_VERSION_MINOR}.${librt_VERSION_PATCH}
    SOVERSION  ${librt_VERSION_MAJOR}
)
set_target_properties (
    rt            PROPERTIES
    COMPILE_FLAGS "${RT_COMPILER_FLAGS}"
)

install (
    TARGETS rt
    RUNTIME DESTINATION lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install (
    DIRECTORY ${librt_PUBLIC_HEADERS}/
    DESTINATION include/librt
)
