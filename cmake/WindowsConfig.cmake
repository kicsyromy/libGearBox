if (${CMAKE_SYSTEM_NAME} MATCHES "Windows" OR ${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore")
    function (libgearbox_compile_definitions RESULT)
        set (COMPILE_DEFINITIONS "")

        if (${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore")
            list (APPEND COMPILE_DEFINITIONS "-DPLATFORM_UWP")
            list (APPEND COMPILE_DEFINITIONS "-DPLATFORM_WINDOWS")
        elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
            list (APPEND COMPILE_DEFINITIONS "-DPLATFORM_WIN32")
            list (APPEND COMPILE_DEFINITIONS "-DPLATFORM_WINDOWS")
        endif ()

        set (${RESULT} ${COMPILE_DEFINITIONS} PARENT_SCOPE)
    endfunction ()

    function (libgearbox_compiler_flags RESULT)
        set (COMPILER_FLAGS "")

        if (DEFINED MSVC)
            list (APPEND COMPILER_FLAGS "/wd4251")
        endif ()

        if (${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore")
            list (APPEND COMPILER_FLAGS "/FI")
            list (APPEND COMPILER_FLAGS "/ZW")
        endif ()

        string (REPLACE ";" " " COMPILER_FLAGS "${COMPILER_FLAGS}")
        set (${RESULT} ${COMPILER_FLAGS} PARENT_SCOPE)
    endfunction ()

    function (libgearbox_include_dirs RESULT)
        set (${RESULT} "" PARENT_SCOPE)
    endfunction ()

    function (libgearbox_libs RESULT)
        set (${RESULT} "wininet.lib" PARENT_SCOPE)
    endfunction ()
        
    set (LIBGEARBOX_GENERATE_MANPAGE "NO")
endif ()