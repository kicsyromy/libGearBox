if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions (-DPLATFORM_LINUX)
    set (PLATFORM_OK TRUE)

    set (COMPILER_FLAGS "${COMPILER_FLAGS} -fPIC")

    # Add CURL dependency
    find_package (PkgConfig)
    pkg_search_module (CURL REQUIRED libcurl)
    set (INCLUDE_DIRS ${CURL_LIBRARIES})
    set (LIB_DIRS ${CURL_LIBRARY_DIRS})
    set (LIBS ${CURL_LIBRARIES})
    set (COMPILER_FLAGS "${COMPILER_FLAGS} ${CURL_CFLAGS}")
endif ()

