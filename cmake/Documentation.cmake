find_package (Doxygen)
if (DOXYGEN_FOUND)
    set (GEARBOX_DOC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/docs/")
    set (GEARBOX_STYLE_SHEET "${GEARBOX_DOC_FILES}/style.css")
    configure_file ("${GEARBOX_DOC_FILES}/Doxyfile.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)
    add_custom_target (
        docs ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY     ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation" VERBATIM
    )
else ()
    message (WARNING "Documentation build was requested but Doxygen binary was not found")
endif ()
