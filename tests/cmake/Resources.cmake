function (libgearbox_test_configure_resources RESULT)
    if (${CMAKE_SYSTEM_NAME} MATCHES "WindowsStore")
        file (GLOB RESOURCES "${PROJECT_SOURCE_DIR}/resources/*")
        source_group("Assets" FILES ${RESOURCES})

        set (LIBGEARBOX_TEST_APPLICATION_TITLE "libGearBox Tests")
        set (LIBGEARBOX_TEST_APPLICATION_DESCRIPTION "Tests for libGearBox Transmission Remote client library")
        set (LIBGEARBOX_TEST_ORGANIZATION "kicsyromy")

        configure_file (
            "${PROJECT_SOURCE_DIR}/package.appxmanifest.in"
            "${PROJECT_BINARY_DIR}/package.appxmanifest"
            @ONLY
        )

        list (APPEND RESOURCES "${PROJECT_BINARY_DIR}/package.appxmanifest")

        set_property (SOURCE ${RESOURCES} PROPERTY VS_DEPLOYMENT_CONTENT 1)
        set_property (SOURCE ${RESOURCES} PROPERTY VS_DEPLOYMENT_LOCATION "resources")
    else ()
        set (RESOURCES "")
    endif ()

    set (${RESULT} ${RESOURCES} PARENT_SCOPE)
endfunction ()
