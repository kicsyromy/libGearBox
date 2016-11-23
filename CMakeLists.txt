cmake_minimum_required(VERSION 2.8.12)
project(librt)
add_definitions(-DRT_LIB)

# Version number
set(librt_VERSION_MAJOR 0)
set(librt_VERSION_MINOR 0)
set(librt_VERSION_PATCH 3)
add_definitions(-DLIBRT_VERISION=${librt_VERSION_MAJOR}${LIBRT_VERSION_MINOR}${LIBRT_VERSION_PATCH})
add_definitions(-DLIBRT_VERISION_STR="${librt_VERSION_MAJOR}.${librt_VERSION_MINOR}.${librt_VERSION_PATCH}")

set(librt_PRIVATE_HEADERS "${PROJECT_SOURCE_DIR}/src/inc")
set(librt_PUBLIC_HEADERS  "${PROJECT_SOURCE_DIR}/pub")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_BUILD)
    message(STATUS "Building with debug informaton and logging")
endif()

# Show header files in the project structure
FILE(GLOB_RECURSE PRIVATE_HEADERS "${librt_PRIVATE_HEADERS}/*.h")
add_custom_target(private_headers SOURCES ${PRIVATE_HEADERS})
FILE(GLOB_RECURSE PUBLIC_HEADERS "${librt_PUBLIC_HEADERS}/*.h")
add_custom_target(public_headers SOURCES ${PUBLIC_HEADERS})
FILE(GLOB_RECURSE JSON_HEADERS "${PROJECT_SOURCE_DIR}/3rdparty/json/src/*.hpp")
add_custom_target(json_headers SOURCES ${JSON_HEADERS})
FILE(GLOB_RECURSE SEQ_HEADERS "${PROJECT_SOURCE_DIR}/3rdparty/sequential/*.h")
add_custom_target(seq_headers SOURCES ${SEQ_HEADERS})

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-DPLATFORM_WINDOWS)
    set(INCLUDE_DIRS "")
    set(LIB_DIRS "")
    set(LIBS "wininet.lib")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DPLATFORM_LINUX)
    # Make sure compiler support C++14 standard
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
    CHECK_CXX_COMPILER_FLAG("-std=c++1y" COMPILER_SUPPORTS_CXX1Y)
    if(COMPILER_SUPPORTS_CXX14)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    elseif(COMPILER_SUPPORTS_CXX1Y)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
    else()
        message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
    endif()

    # Search for CURL in system paths
    find_package(PkgConfig)
    pkg_search_module(CURL REQUIRED libcurl)
    set(INCLUDE_DIRS ${CURL_LIBRARIES})
    set(LIB_DIRS ${CURL_LIBRARY_DIRS})
    set(LIBS ${CURL_LIBRARIES})

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CURL_CFLAGS}-fPIC")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DPLATFORM_MACOS)
    # Do some macOS magic here
else()
    message(FATAL_ERROR "The platform your are trying to compile for is not supported")
endif()

include_directories(SYSTEM ${INCLUDE_DIRS})
link_directories(${LIB_DIRS})

# Add C++ Format dependency
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/fmt")
set(FMT_SOURCE "${PROJECT_SOURCE_DIR}/3rdparty/fmt/fmt/format.cc")

# Add json dependency
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/json/src")

# Add sequential dependency
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/sequential")

# Add additional include directories
include_directories("${PROJECT_SOURCE_DIR}/src/inc")
include_directories("${PROJECT_SOURCE_DIR}/pub")

aux_source_directory(${PROJECT_SOURCE_DIR}/src LIBRT_SOURCES)

if (BUILD_SHARED STREQUAL "YES")
    add_library(rt SHARED ${LIBRT_SOURCES} ${FMT_SOURCE})
    message(STATUS "Building shared library")
else()
    add_library(rt STATIC ${LIBRT_SOURCES} ${FMT_SOURCE})
    message(STATUS "Building static library")
endif()
target_link_libraries(rt ${LIBS})
set_target_properties(rt PROPERTIES VERSION ${librt_VERSION_MAJOR}.${librt_VERSION_MINOR}.${librt_VERSION_PATCH} SOVERSION ${librt_VERSION_MAJOR})

install(
    TARGETS rt
    RUNTIME DESTINATION lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY ${librt_PUBLIC_HEADERS}/
    DESTINATION include/librt
)
