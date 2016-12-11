if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions (-DPLATFORM_LINUX)
    set (RT_PLATFORM_OK TRUE)

    set (RT_COMPILER_FLAGS "${RT_COMPILER_FLAGS} -fPIC")

    # Add CURL dependency
    find_package (PkgConfig)
    pkg_search_module (CURL REQUIRED libcurl)
    set (RT_INCLUDE_DIRS ${CURL_LIBRARIES})
    set (RT_LIB_DIRS ${CURL_LIBRARY_DIRS})
    set (RT_LIBS ${CURL_LIBRARIES})
    set (RT_COMPILER_FLAGS "${RT_COMPILER_FLAGS} ${CURL_CFLAGS}")
endif ()

