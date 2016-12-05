if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions (-DPLATFORM_WINDOWS)
    set (PLATFORM_OK TRUE)

    set (COMPILER_FLAGS "")

    set (INCLUDE_DIRS "")
    set (LIB_DIRS "")
    set (LIBS "wininet.lib")
endif ()

