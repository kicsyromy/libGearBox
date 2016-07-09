cmake_minimum_required(VERSION 2.8.12)
project(librt)
add_definitions(-DRT_LIB)

# Make sure compiler support C++14 standard
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++1y" COMPILER_SUPPORTS_CXX1Y)
if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif(COMPILER_SUPPORTS_CXX1Y)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Version number
set(librt_VERSION_MAJOR 0)
set(librt_VERSION_MINOR 0)
set(librt_VERSION_PATCH 2)

set(librt_PRIVATE_HEADERS "${PROJECT_SOURCE_DIR}/src/inc")
set(librt_PUBLIC_HEADERS  "${PROJECT_SOURCE_DIR}/pub")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_BUILD)
endif()

# Hack for Qt Creator to show the headers in the project file tree
FILE(GLOB_RECURSE PRIVATE_HEADERS "${librt_PRIVATE_HEADERS}/*.h")
add_custom_target(private_headers SOURCES ${PRIVATE_HEADERS})
FILE(GLOB_RECURSE PUBLIC_HEADERS "${librt_PUBLIC_HEADERS}/*.h")
add_custom_target(public_headers SOURCES ${PUBLIC_HEADERS})
FILE(GLOB_RECURSE JSON_HEADERS "${PROJECT_SOURCE_DIR}/3rdparty/json/src/*.hpp")
add_custom_target(json_headers SOURCES ${JSON_HEADERS})
FILE(GLOB_RECURSE SEQ_HEADERS "${PROJECT_SOURCE_DIR}/3rdparty/sequential/*.h")
add_custom_target(seq_headers SOURCES ${SEQ_HEADERS})

# Add CURL Requests dependency
find_package(CURL)
include_directories("${CURL_INCLUDE_DIRS}")

# Add C++ Format dependency
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/fmt")
set(FMT_SOURCE "${PROJECT_SOURCE_DIR}/3rdparty/fmt/fmt/format.cc")

#Add json dependency
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/json/src")

#Add sequential dependency
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/sequential")

# Specify project include directories
include_directories("${PROJECT_SOURCE_DIR}/src/inc")
include_directories("${PROJECT_SOURCE_DIR}/pub")

# Define the source directories
aux_source_directory(${PROJECT_SOURCE_DIR}/src LIBRT_SOURCES)

# Specify the target
if (BUILD_SHARED STREQUAL "YES")
    add_library(rt SHARED ${LIBRT_SOURCES} ${FMT_SOURCE})
    message("-- Building shared library")
else()
    add_library(rt STATIC ${LIBRT_SOURCES} ${FMT_SOURCE})
    message("-- Building static library")
endif()
# Se the apropriate compiler flags based on output from pkg-config
target_link_libraries(rt ${CURL_LIBRARIES})
# Create aditional files with versioning
set_target_properties(rt PROPERTIES VERSION ${librt_VERSION_MAJOR}.${librt_VERSION_MINOR}.${librt_VERSION_PATCH} SOVERSION ${librt_VERSION_MAJOR})

# Install binaries
install(
    TARGETS rt
    RUNTIME DESTINATION lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install public headers
install(
    DIRECTORY ${librt_PUBLIC_HEADERS}/
    DESTINATION include/librt
)
