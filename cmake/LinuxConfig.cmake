if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    function (libgearbox_compile_definitions RESULT)
        set (${RESULT} "-DPLATFORM_LINUX" PARENT_SCOPE)
    endfunction ()

    find_package (PkgConfig QUIET REQUIRED)
    pkg_search_module (CURL REQUIRED libcurl)

    function (libgearbox_compiler_flags RESULT)
        set (COMPILER_FLAGS "-fPIC")

        list (APPEND COMPILER_FLAGS "${CURL_CFLAGS}")

        string (REPLACE ";" " " COMPILER_FLAGS "${COMPILER_FLAGS}")
        set (${RESULT} ${COMPILER_FLAGS} PARENT_SCOPE)
    endfunction ()

    function (libgearbox_include_dirs RESULT)
        set (${RESULT} ${CURL_INCLUDE_DIRS} PARENT_SCOPE)
    endfunction ()

    function (libgearbox_libs RESULT)
        set (${RESULT} ${CURL_LIBRARIES} PARENT_SCOPE)
    endfunction ()
        
    set (LIBGEARBOX_GENERATE_MANPAGE "YES")
endif ()
