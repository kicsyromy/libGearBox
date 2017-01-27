project (librt_test)

include_directories ("${PROJECT_SOURCE_DIR}/src/inc")
set (LIBRT_TEST_HEADERS_DIR "${PROJECT_SOURCE_DIR}/src/inc")
file (GLOB_RECURSE LIBRT_TEST_HEADERS "${LIBRT_TEST_HEADERS_DIR}/*.h")
add_custom_target (librt_test_headers SOURCES ${LIBRT_TEST_HEADERS})

## CATCH ##
include_directories ("${PROJECT_SOURCE_DIR}/3rdparty/catch")

## librt PRIVATE HEADERS ##
include_directories ("${PROJECT_SOURCE_DIR}/../src/include")

## librt SOURCES ##
include_directories ("${PROJECT_SOURCE_DIR}/../src")

## librt PUBLIC HEADERS ##
include_directories ("${PROJECT_SOURCE_DIR}/../include")

## SOURCES ##
aux_source_directory (${PROJECT_SOURCE_DIR}/src LIBRT_TEST_SOURCES)

add_executable (librt_test ${LIBRT_TEST_SOURCES} ${FMT_SOURCE})
target_link_libraries (librt_test ${RT_LIBS})
set_target_properties (
    librt_test    PROPERTIES
    COMPILE_FLAGS "-std=c++14"
)
