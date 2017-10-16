cmake_minimum_required (VERSION 3.0)
project (libgearboxtest)
list (APPEND CMAKE_MODULE_PATH 1 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option (GEARBOX_TESTS_ENABLE_CODE_COVERAGE "Generate test coverage report." OFF)

## TEST HEADERS ##
file (GLOB_RECURSE LIBGEARBOX_TEST_HEADERS "${PROJECT_SOURCE_DIR}/src/include/*.h")
source_group ("Header Files" FILES ${LIBGEARBOX_TEST_HEADERS})

## SOURCES ##
file (GLOB_RECURSE LIBGEARBOX_TEST_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
source_group ("Source Files" FILES ${LIBGEARBOX_TEST_SERVER_JSON})

## PYTHON TEST SERVER SOURCES ##
file (GLOB_RECURSE LIBGEARBOX_TEST_SERVER_SOURCES "${PROJECT_SOURCE_DIR}/python/*.py")
file (GLOB_RECURSE LIBGEARBOX_TEST_SERVER_JSON "${PROJECT_SOURCE_DIR}/python/json/*.json")
source_group ("Server Files" FILES ${LIBGEARBOX_TEST_SERVER_SOURCES})
source_group ("Server Files\\Json"   FILES ${LIBGEARBOX_TEST_SERVER_JSON})

## RESOURCES ##
include (Resources)
libgearbox_test_configure_resources (LIBGEARBOX_TEST_RESOURCES)

if (GEARBOX_TESTS_ENABLE_CODE_COVERAGE)
    include (Coverage)
endif ()

add_executable (
    ${PROJECT_NAME}
    ${LIBGEARBOX_TEST_SOURCES} ${LIBGEARBOX_TEST_HEADERS}
    ${LIBGEARBOX_TEST_SERVER_SOURCES} ${LIBGEARBOX_TEST_SERVER_JSON}
    ${LIBGEARBOX_TEST_RESOURCES}
)

foreach (DEFINITION IN LISTS LIBGEARBOX_COMPILE_DEFINITIONS)
    target_compile_definitions (${PROJECT_NAME} PRIVATE "${DEFINITION}")
endforeach ()

target_compile_definitions (${PROJECT_NAME} PRIVATE "-DGEARBOX_PYTHON_MODULE_PATH=\"${PROJECT_SOURCE_DIR}/python\"")

## HEADERS ##
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src/include")

## CATCH ##
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/catch")

## LIBGEARBOX ##
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../include")
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../src/include")
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../src")
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_BINARY_DIR}/../config")

## NLOHMANN JSON ##
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../3rdparty/json/src")

## SEQUENTIAL SERIALIZER ##
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../3rdparty/sequential")

## C++ FORMAT ##
target_compile_definitions (${PROJECT_NAME} PRIVATE "-DFMT_HEADER_ONLY")
target_include_directories (${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../3rdparty/fmt")

## PYTHON ##
include (Python)
libgearbox_test_find_python_binary (LIBGEARBOX_TEST_PY_BIN)
libgearbox_test_find_python_include_dirs (LIBGEARBOX_TEST_PY_INCLUDE_DIRS)
libgearbox_test_find_python_libraries (LIBGEARBOX_TEST_PY_LIBRARIES)

target_include_directories (${PROJECT_NAME} PRIVATE ${LIBGEARBOX_TEST_PY_INCLUDE_DIRS})

target_link_libraries (
    ${PROJECT_NAME}
    ${LIBGEARBOX_LIBS} ${LIBGEARBOX_TEST_PY_LIBRARIES} ${GEARBOX_TESTS_COVERAGE_LIBRARIES}
)

include (Platform)
libgearbox_test_platform_libraries (LIBGEARBOX_TEST_PLATFORM_LIBRARIES)

target_link_libraries (
    ${PROJECT_NAME}
    ${LIBGEARBOX_TEST_PLATFORM_LIBRARIES}
)

set_target_properties (
    ${PROJECT_NAME}       PROPERTIES
    CXX_STANDARD          14
    CXX_STANDARD_REQUIRED YES
    COMPILE_FLAGS         "${LIBGEARBOX_COMPILER_FLAGS} ${GEARBOX_TESTS_COVERAGE_COMPILER_FLAGS}"
)

libgearbox_test_create_platform_target (${LIBGEARBOX_TEST_PY_BIN} ${PROJECT_SOURCE_DIR}/python)
