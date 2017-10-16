function (libgearbox_test_platform_libraries RESULT)
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set (${RESULT} "-lpthread" PARENT_SCOPE)
    endif()
endfunction ()

macro (libgearbox_test_create_platform_target PYTHON_BINARY PYTHON_MODULE_PATH)
    add_custom_target (
        check
        COMMAND ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endmacro()
