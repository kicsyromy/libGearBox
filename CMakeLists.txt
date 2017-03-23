cmake_minimum_required (VERSION 2.8.12)
project (libgearbox)
add_definitions (-DGEARBOX_LIB)

set (libgearbox_VERSION_MAJOR 0)
set (libgearbox_VERSION_MINOR 1)
set (libgearbox_VERSION_PATCH 0)
add_definitions (-DLIBGEARBOX_VERISION=${libgearbox_VERSION_MAJOR}${libgearbox_VERSION_MINOR}${libgearbox_VERSION_PATCH})
add_definitions (-DLIBGEARBOX_VERISION_STR="${libgearbox_VERSION_MAJOR}.${libgearbox_VERSION_MINOR}.${libgearbox_VERSION_PATCH}")

option (GEARBOX_STATIC_LIB "Build static library if set to ON otherwise build shared library." ON)
option (GEARBOX_DEBUG_LOGS "Build with additional debug logs. CAUTION: Do not use in production." OFF)
option (GEARBOX_ALLOW_UNSUPPORTED_COMPILER "Allow compilation using an unsupported compiler." OFF)
option (GEARBOX_ALLOW_UNSUPPORTED_PLATFORM "Allow compilation on an unsupported platform." OFF)
option (GEARBOX_ENABLE_TESTS "Build tests." OFF)
option (GEARBOX_BUILD_DOCUMENTATION "Build documentation." OFF)

list (APPEND CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(LinuxConfig)
include(macOSConfig)
include(WindowsConfig)
include(TestCompilerSupport)

if (NOT GEARBOX_PLATFORM_OK)
    message (WARNING "Building on unsupported platform")
    if (NOT GEARBOX_ALLOW_UNSUPPORTED_PLATFORM)
        message (FATAL_ERROR "Stopping since RT_ALLOW_UNSUPPORTED_PLATFORM is not set")
    endif ()
endif ()

## PRIVATE HEADERS ##
include_directories ("${PROJECT_SOURCE_DIR}/src/include")
set (libgearbox_PRIVATE_HEADERS "${PROJECT_SOURCE_DIR}/src/include")
file (GLOB_RECURSE PRIVATE_HEADERS "${libgearbox_PRIVATE_HEADERS}/*.h")
add_custom_target (private_headers SOURCES ${PRIVATE_HEADERS})

## PUBLIC HEADERS ##
include_directories ("${PROJECT_SOURCE_DIR}/include")
set (libgearbox_PUBLIC_HEADERS  "${PROJECT_SOURCE_DIR}/include")
file (GLOB_RECURSE PUBLIC_HEADERS "${libgearbox_PUBLIC_HEADERS}/*.h")
add_custom_target (public_headers SOURCES ${PUBLIC_HEADERS})

## NLOHMANN JSON ##
include_directories ("${PROJECT_SOURCE_DIR}/3rdparty/json/src")

## SEQUENTIAL SERIALIZER ##
include_directories ("${PROJECT_SOURCE_DIR}/3rdparty/sequential")

## C++ FORMAT ##
add_definitions (-DFMT_HEADER_ONLY)
include_directories ("${PROJECT_SOURCE_DIR}/3rdparty/fmt")

include_directories (SYSTEM ${GEARBOX_INCLUDE_DIRS})
link_directories (${GEARBOX_LIB_DIRS})

aux_source_directory ("${PROJECT_SOURCE_DIR}/src" LIBGEARBOX_SOURCES)

if (GEARBOX_BUILD_DOCUMENTATION)
    set (GEARBOX_SOURCES "${PROJECT_SOURCE_DIR}/src")
    include (Documentation)
endif ()

if (GEARBOX_DEBUG_LOGS)
    add_definitions (-DGEARBOX_LOG_DEBUG)
    message (STATUS "Building with debug informaton and logging")
endif ()

if (GEARBOX_STATIC_LIB)
    add_library (gearbox STATIC ${LIBGEARBOX_SOURCES})
    message (STATUS "Building static library")
else ()
    add_library (gearbox SHARED ${LIBGEARBOX_SOURCES})
    message (STATUS "Building shared library")
endif ()

target_link_libraries (gearbox ${GEARBOX_LIBS})
set_target_properties (
    gearbox       PROPERTIES
    COMPILE_FLAGS "${GEARBOX_COMPILER_FLAGS}"
    VERSION       ${libgearbox_VERSION_MAJOR}.${libgearbox_VERSION_MINOR}.${libgearbox_VERSION_PATCH}
    SOVERSION     ${libgearbox_VERSION_MAJOR}
)

install (
    TARGETS gearbox
    RUNTIME DESTINATION lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install (
    DIRECTORY ${libgearbox_PUBLIC_HEADERS}/
    DESTINATION include/gearbox
)

if (GEARBOX_ENABLE_TESTS)
    add_subdirectory (tests)
endif()
