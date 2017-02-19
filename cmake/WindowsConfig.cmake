if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions (-DPLATFORM_WINDOWS)
    set (GEARBOX_PLATFORM_OK TRUE)

    set (GEARBOX_COMPILER_FLAGS "")

    set (GEARBOX_INCLUDE_DIRS "")
    set (GEARBOX_LIB_DIRS "")
    set (GEARBOX_LIBS "wininet.lib")
	set (GEARBOX_GENERATE_MANPAGE "NO")
endif ()

