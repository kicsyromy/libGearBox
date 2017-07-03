find_package (Doxygen)
if (DOXYGEN_FOUND)
    set (GEARBOX_DOC_FILES "${PROJECT_SOURCE_DIR}/docs/")
    set (GEARBOX_STYLE_SHEET "${GEARBOX_DOC_FILES}/style.css")
    configure_file ("${GEARBOX_DOC_FILES}/Doxyfile.in" "${PROJECT_BINARY_DIR}/Doxyfile" @ONLY)
    add_custom_target (
        docs ALL
        ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY     ${PROJECT_BINARY_DIR}
        COMMENT "Generating API documentation" VERBATIM
    )
else ()
    message (WARNING "Documentation build was requested but Doxygen binary was not found")
endif ()
