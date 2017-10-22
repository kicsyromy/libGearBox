function (libgearbox_generate_documentation GENERATE)
    if (${GENERATE})
        find_package (Doxygen)
        if (DOXYGEN_FOUND)
            set (LIBGEARBOX_PUBLIC_HEADERS_PATH  "${PROJECT_SOURCE_DIR}/include")
            set (LIBGEARBOX_PRIVATE_HEADERS_PATH "${PROJECT_SOURCE_DIR}/src/include")
            set (LIBGEARBOX_SOURCES_PATH         "${PROJECT_SOURCE_DIR}/src")
            set (LIBGEARBOX_DOC_FILES            "${PROJECT_SOURCE_DIR}/docs")
            set (LIBGEARBOX_STYLE_SHEET          "${LIBGEARBOX_DOC_FILES}/style.css")
            configure_file ("${LIBGEARBOX_DOC_FILES}/Doxyfile.in" "${PROJECT_BINARY_DIR}/Doxyfile" @ONLY)
            add_custom_target (
                docs ALL
                ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
                WORKING_DIRECTORY     ${PROJECT_BINARY_DIR}
                COMMENT "Generating API documentation" VERBATIM
            )
        else ()
            message (WARNING "Documentation build was requested but Doxygen binary was not found")
        endif ()
    endif ()
endfunction ()
