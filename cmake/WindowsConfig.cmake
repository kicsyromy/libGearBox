if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions (-DPLATFORM_WINDOWS)
    set (GEARBOX_PLATFORM_OK TRUE)

	if (DEFINED MSVC)
		set (GEARBOX_COMPILER_FLAGS "/wd4251")
	endif ()

    set (GEARBOX_INCLUDE_DIRS "")
    set (GEARBOX_LIB_DIRS "")
    set (GEARBOX_LIBS "wininet.lib")
	set (GEARBOX_GENERATE_MANPAGE "NO")
endif ()

