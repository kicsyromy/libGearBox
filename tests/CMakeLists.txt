project (libgearbox_test)

option (GEARBOX_TESTS_ENABLE_CODE_COVERAGE "Generate test coverage report." OFF)

list (APPEND CMAKE_MODULE_PATH 1 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(PythonConfig)

## TEST HEADERS ##
include_directories ("${PROJECT_SOURCE_DIR}/src/include")
set (LIBGEARBOX_TEST_HEADERS_DIR "${PROJECT_SOURCE_DIR}/src/include")
file (GLOB_RECURSE LIBGEARBOX_TEST_HEADERS "${LIBGEARBOX_TEST_HEADERS_DIR}/*.h")
add_custom_target (libgearbox_test_headers SOURCES ${LIBGEARBOX_TEST_HEADERS})

## PYTHON TEST SERVER SOURCES ##
set (LIBGEARBOX_TEST_SERVER_DIR "${PROJECT_SOURCE_DIR}/python")
file (GLOB_RECURSE LIBGEARBOX_TEST_SERVER_SOURCES "${LIBGEARBOX_TEST_SERVER_DIR}/*.py")
file (GLOB_RECURSE LIBGEARBOX_TEST_SERVER_JSON "${LIBGEARBOX_TEST_SERVER_DIR}/json/*.json")
add_custom_target (libgearbox_test_server SOURCES ${LIBGEARBOX_TEST_SERVER_SOURCES} ${LIBGEARBOX_TEST_SERVER_JSON})
add_definitions (-DGEARBOX_PYTHON_MODULE_PATH="${LIBGEARBOX_TEST_SERVER_DIR}")

## CATCH ##
include_directories ("${PROJECT_SOURCE_DIR}/3rdparty/catch")

## libgearbox SOURCES ##
include_directories ("${PROJECT_SOURCE_DIR}/../src")

# PYTHON DIRECTORIES
include_directories (SYSTEM ${GEARBOX_PY_INCLUDE_DIRS})
link_directories (${PYTHON_LIBRARY_DIRS})

## SOURCES ##
aux_source_directory ("${PROJECT_SOURCE_DIR}/src" LIBGEARBOX_TEST_SOURCES)

if (GEARBOX_TESTS_ENABLE_CODE_COVERAGE)
    include (Coverage)
endif ()

add_executable (${PROJECT_NAME} ${LIBGEARBOX_TEST_SOURCES})
target_link_libraries (${PROJECT_NAME} ${GEARBOX_LIBS} ${GEARBOX_PY_LIBS} ${GEARBOX_TESTS_COVERAGE_LIBRARIES})
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries (${PROJECT_NAME} -lpthread)
endif()

set_target_properties (
    ${PROJECT_NAME} PROPERTIES
    COMPILE_FLAGS   "${GEARBOX_COMPILER_FLAGS} ${GEARBOX_TESTS_COVERAGE_COMPILER_FLAGS}"
)

add_custom_target (
    check
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
