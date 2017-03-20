find_program (GEARBOX_TESTS_GCOV gcov)
find_program (GEARBOX_TESTS_LCOV lcov)
find_program (GEARBOX_TESTS_GENHTML genhtml)

if (NOT GEARBOX_TESTS_GCOV)
    message (WARNING "Failed to find gcov binary")
else ()
    set (GEARBOX_TESTS_GCOV_FOUND 1)
endif ()

if (NOT GEARBOX_TESTS_LCOV)
    message (WARNING "Failed to find lcov binary")
else ()
    set (GEARBOX_TESTS_LCOV_FOUND 1)
endif ()

if (NOT GEARBOX_TESTS_GENHTML)
    message (WARNING "Failed to find genhtml binary")
else ()
    set (GEARBOX_TESTS_GENHTML_FOUND 1)
endif ()

if ((NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang") AND (NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
    message (WARNING "${CMAKE_CXX_COMPILER} cannot be used for code coverage reporting. Possible alternatives are Clang, GCC, or MinGW")
else ()
    set (GEARBOX_TESTS_COVERAGE_COMPILER_OK 1)
endif ()

if (GEARBOX_TESTS_GCOV_FOUND AND GEARBOX_TESTS_LCOV_FOUND AND GEARBOX_TESTS_GENHTML_FOUND AND GEARBOX_TESTS_COVERAGE_COMPILER_OK)
    set (GEARBOX_TESTS_COVERAGE_COMPILER_FLAGS "-fprofile-arcs -ftest-coverage")
    set (GEARBOX_TESTS_COVERAGE_LIBRARIES gcov)

    set (GEARBOX_TESTS_COVERAGE_OUTPUT_NAME coverage)

    add_custom_target (
        coverage
        ${GEARBOX_TESTS_LCOV} --directory . --zerocounters
        COMMAND ${PROJECT_NAME}
        COMMAND ${GEARBOX_TESTS_LCOV} --directory . --capture --output-file ${GEARBOX_TESTS_COVERAGE_OUTPUT_NAME}.info
        COMMAND ${GEARBOX_TESTS_LCOV} --remove ${GEARBOX_TESTS_COVERAGE_OUTPUT_NAME}.info '*format*' '*json*' '*sequential*' 'catch.hpp' 'main.cpp' 'http_server*' 'test_*' '/usr/*' --output-file ${GEARBOX_TESTS_COVERAGE_OUTPUT_NAME}.info.cleaned
        COMMAND ${GEARBOX_TESTS_GENHTML} -o ${GEARBOX_TESTS_COVERAGE_OUTPUT_NAME} ${GEARBOX_TESTS_COVERAGE_OUTPUT_NAME}.info.cleaned --legend -s
        COMMAND ${CMAKE_COMMAND} -E remove ${GEARBOX_TESTS_COVERAGE_OUTPUT_NAME}.info ${GEARBOX_TESTS_COVERAGE_OUTPUT_NAME}.info.cleaned
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
else ()
    message (WARNING "Code coverage reporting disabled due to previous warnings")
endif ()
