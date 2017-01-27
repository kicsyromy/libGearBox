project (libgearbox_test)

include_directories ("${PROJECT_SOURCE_DIR}/src/inc")
set (LIBGEARBOX_TEST_HEADERS_DIR "${PROJECT_SOURCE_DIR}/src/inc")
file (GLOB_RECURSE LIBGEARBOX_TEST_HEADERS "${LIBGEARBOX_TEST_HEADERS_DIR}/*.h")
add_custom_target (libgearbox_test_headers SOURCES ${LIBGEARBOX_TEST_HEADERS})

## CATCH ##
include_directories ("${PROJECT_SOURCE_DIR}/3rdparty/catch")

## libgearbox PRIVATE HEADERS ##
include_directories ("${PROJECT_SOURCE_DIR}/../src/include")

## libgearbox SOURCES ##
include_directories ("${PROJECT_SOURCE_DIR}/../src")

## libgearbox PUBLIC HEADERS ##
include_directories ("${PROJECT_SOURCE_DIR}/../include")

## SOURCES ##
aux_source_directory (${PROJECT_SOURCE_DIR}/src LIBGEARBOX_TEST_SOURCES)

add_executable (${PROJECT_NAME} ${LIBGEARBOX_TEST_SOURCES} ${FMT_SOURCE})
target_link_libraries (${PROJECT_NAME} ${GEARBOX_LIBS})
set_target_properties (
    ${PROJECT_NAME} PROPERTIES
    COMPILE_FLAGS   "-std=c++14"
)
