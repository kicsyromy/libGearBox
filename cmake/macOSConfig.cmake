if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions (-DPLATFORM_MACOS)
    set (GEARBOX_PLATFORM_OK TRUE)

    # Enable Objective C++ and ARC
    set (GEARBOX_COMPILER_FLAGS "${GEARBOX_COMPILER_FLAGS} -x objective-c++ -fobjc-arc")

    set (GEARBOX_INCLUDE_DIRS "")
    set (GEARBOX_LIB_DIRS "")
    set (GEARBOX_LIBS "-framework Cocoa")
endif ()
