if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions (-DPLATFORM_LINUX)
    set (GEARBOX_PLATFORM_OK TRUE)

    set (GEARBOX_COMPILER_FLAGS "${GEARBOX_COMPILER_FLAGS} -fPIC")

    # Add CURL dependency
    find_package (PkgConfig QUIET REQUIRED)
    pkg_search_module (CURL REQUIRED libcurl)
    set (GEARBOX_INCLUDE_DIRS ${CURL_LIBRARIES})
    set (GEARBOX_LIB_DIRS ${CURL_LIBRARY_DIRS})
    set (GEARBOX_LIBS ${CURL_LIBRARIES})
    set (GEARBOX_COMPILER_FLAGS "${GEARBOX_COMPILER_FLAGS} ${CURL_CFLAGS}")
    set (GEARBOX_GENERATE_MANPAGE "YES")
endif ()

