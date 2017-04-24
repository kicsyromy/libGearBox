find_program(CLANG_FORMAT clang-format NAMES clang-format clang-format-3.8 clang-format-3.9)
if (NOT CLANG_FORMAT-NOTFOUND)
    add_custom_target (
        clangformat
        COMMAND ${CLANG_FORMAT}
        -style=file
        -i "${PROJECT_SOURCE_DIR}/src/include/*.h" "${PROJECT_SOURCE_DIR}/include/*.h" "${PROJECT_SOURCE_DIR}/src/*.cpp"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endif ()
