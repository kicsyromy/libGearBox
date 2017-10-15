project (libgearbox_test)

option (GEARBOX_TESTS_ENABLE_CODE_COVERAGE "Generate test coverage report." OFF)

list (APPEND CMAKE_MODULE_PATH 1 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(PythonConfig)

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

if (GEARBOX_TESTS_ENABLE_CODE_COVERAGE)
    include (Coverage)
endif ()

add_executable (
    ${PROJECT_NAME}
    ${LIBGEARBOX_TEST_SOURCES} ${LIBGEARBOX_TEST_HEADERS}
    ${LIBGEARBOX_TEST_SERVER_SOURCES} ${LIBGEARBOX_TEST_SERVER_JSON}
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

## PYTHON DIRECTORIES ##
target_include_directories (${PROJECT_NAME} PRIVATE ${GEARBOX_PY_INCLUDE_DIRS})
link_directories (${PYTHON_LIBRARY_DIRS}) # Need to find a better way to set library dirs

target_link_libraries (
    ${PROJECT_NAME}
    ${LIBGEARBOX_LIBS} ${GEARBOX_PY_LIBS} ${GEARBOX_TESTS_COVERAGE_LIBRARIES}
)
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries (
        ${PROJECT_NAME}
        -lpthread
    )
endif()

set_target_properties (
    ${PROJECT_NAME} PROPERTIES
    COMPILE_FLAGS   "${LIBGEARBOX_COMPILER_FLAGS} ${GEARBOX_TESTS_COVERAGE_COMPILER_FLAGS}"
)

add_custom_target (
    check
    COMMAND ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
