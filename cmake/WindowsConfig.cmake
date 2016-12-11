if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions (-DPLATFORM_WINDOWS)
    set (RT_PLATFORM_OK TRUE)

    set (RT_COMPILER_FLAGS "")

    set (RT_INCLUDE_DIRS "")
    set (RT_LIB_DIRS "")
    set (RT_LIBS "wininet.lib")
endif ()

