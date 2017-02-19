find_package (Doxygen)
if (DOXYGEN_FOUND)
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target (
        docs ALL
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY     ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation" VERBATIM
    )
else ()
    message (WARNING "Documentation build was requested but Doxygen binary was not found")
endif ()
