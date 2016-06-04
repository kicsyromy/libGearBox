cmake_minimum_required(VERSION 2.8.12)
project(librt)
add_definitions(-DRT_LIB)

# Version number
set(librt_VERSION_MAJOR 0)
set(librt_VERSION_MINOR 0)
set(librt_VERSION_PATCH 1)

set(librt_PRIVATE_HEADERS "${PROJECT_SOURCE_DIR}/src/inc")
set(librt_PUBLIC_HEADERS  "${PROJECT_SOURCE_DIR}/pub")

# Hack for Qt Creator to show the headers in the project file tree
FILE(GLOB_RECURSE PRIVATE_HEADERS "${librt_PRIVATE_HEADERS}/*.h")
add_custom_target(private_headers SOURCES ${PRIVATE_HEADERS})
FILE(GLOB_RECURSE PUBLIC_HEADERS "${librt_PUBLIC_HEADERS}/*.h")
add_custom_target(public_headers SOURCES ${PUBLIC_HEADERS})
FILE(GLOB_RECURSE JSON_HEADERS "${PROJECT_SOURCE_DIR}/3rdparty/json/src/*.hpp")
add_custom_target(json_headers SOURCES ${JSON_HEADERS})
FILE(GLOB_RECURSE SEQ_HEADERS "${PROJECT_SOURCE_DIR}/3rdparty/sequential/*.h")
add_custom_target(seq_headers SOURCES ${SEQ_HEADERS})

# Add C++ Requests dependency
add_definitions(-DINSECURE_CURL)
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/cpr/include")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/cpr/include/cpr")
aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/cpr/cpr CPR_SOURCES)
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
add_library(rt SHARED ${LIBRT_SOURCES} ${FMT_SOURCE} ${CPR_SOURCES})

# Create aditional files with versioning
set_target_properties(rt PROPERTIES VERSION ${librt_VERSION_MAJOR}.${librt_VERSION_MINOR}.${librt_VERSION_PATCH} SOVERSION ${librt_VERSION_MAJOR})

# Se the apropriate compiler flags based on output from pkg-config
target_link_libraries(rt ${CURL_LIBRARIES})

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
