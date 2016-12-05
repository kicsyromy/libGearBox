if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions (-DPLATFORM_MACOS)
    set (PLATFORM_OK TRUE)

    # Enable Objective C++ and ARC
    set (COMPILER_FLAGS "${COMPILER_FLAGS} -x objective-c++ -fobjc-arc")

    set (INCLUDE_DIRS "")
    set (LIB_DIRS "")
    set (LIBS "-framework Cocoa")
endif ()
