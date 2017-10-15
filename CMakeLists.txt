cmake_minimum_required (VERSION 3.0)
project (libgearbox)

set (LIBGEARBOX_VERSION_MAJOR 0)
set (LIBGEARBOX_VERSION_MINOR 1)
set (LIBGEARBOX_VERSION_PATCH 0)

option (LIBGEARBOX_STATIC_LIB "Build static library if set to ON otherwise build shared library." ON)
option (LIBGEARBOX_DEBUG_LOGS "Build with additional debug logs. CAUTION: Do not use in production." OFF)
option (LIBGEARBOX_ENABLE_TESTS "Build tests." OFF)
option (LIBGEARBOX_BUILD_DOCUMENTATION "Build documentation." OFF)
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

list (APPEND CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (LinuxConfig)
include (macOSConfig)
include (WindowsConfig)
include (ClangFormat)

if (LIBGEARBOX_BUILD_DOCUMENTATION)
    include (Documentation)
endif ()

if (LIBGEARBOX_STATIC_LIB)
    set (LIBGEARBOX_USE_IMPORT_SEMANTICS 0)
    add_library (
        ${PROJECT_NAME} STATIC
        ${LIBGEARBOX_SOURCES} ${LIBGEARBOX_PUBLIC_HEADERS} ${LIBGEARBOX_PRIVATE_HEADERS}
    )
    message (STATUS "Building static library")
else ()
    set (LIBGEARBOX_USE_IMPORT_SEMANTICS 1)
    add_library (
        ${PROJECT_NAME} SHARED
        ${LIBGEARBOX_SOURCES} ${LIBGEARBOX_PUBLIC_HEADERS} ${LIBGEARBOX_PRIVATE_HEADERS}
    )
    message (STATUS "Building shared library")
endif ()

configure_file (
    "${PROJECT_SOURCE_DIR}/include/libgearbox_config.h.in"
    "${PROJECT_BINARY_DIR}/config/libgearbox_config.h"
    @ONLY
)

libgearbox_compile_definitions (LIBGEARBOX_COMPILE_DEFINITIONS)
foreach (DEFINITION IN LISTS LIBGEARBOX_COMPILE_DEFINITIONS)
    target_compile_definitions (${PROJECT_NAME} PRIVATE "${DEFINITION}")
endforeach ()

target_compile_definitions (${PROJECT_NAME} PRIVATE "-DGEARBOX_LIB")

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

## SYSTEM DEPENDENCIES ##
libgearbox_include_dirs (LIBGEARBOX_INCLUDE_DIRS)
target_include_directories (${PROJECT_NAME} PRIVATE "${LIBGEARBOX_INCLUDE_DIRS}")

if (LIBGEARBOX_DEBUG_LOGS)
    target_compile_definitions (${PROJECT_NAME} PRIVATE "-DLIBGEARBOX_LOG_DEBUG")
    message (STATUS "Building with debug informaton and logging")
endif ()

libgearbox_libs (LIBGEARBOX_LIBS)
target_link_libraries (${PROJECT_NAME} PUBLIC "${LIBGEARBOX_LIBS}")

libgearbox_compiler_flags (LIBGEARBOX_COMPILER_FLAGS)
set_target_properties (
    ${PROJECT_NAME}       PROPERTIES
    COMPILE_FLAGS         "${LIBGEARBOX_COMPILER_FLAGS}"
    CXX_STANDARD          14
    CXX_STANDARD_REQUIRED YES
    VERSION               ${LIBGEARBOX_VERSION_MAJOR}.${LIBGEARBOX_VERSION_MINOR}.${LIBGEARBOX_VERSION_PATCH}
    SOVERSION             ${LIBGEARBOX_VERSION_MAJOR}
)

install (
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install (
    DIRECTORY ${LIBGEARBOX_PUBLIC_HEADERS_PATH}/
    DESTINATION include/gearbox
)

if (LIBGEARBOX_ENABLE_TESTS)
    add_subdirectory (tests)
endif ()

if (LIBGEARBOX_BUILD_BINDING_LAYER)
    add_subdirectory (bind)
endif ()
