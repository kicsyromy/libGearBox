if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions (-DPLATFORM_MACOS)
    set (RT_PLATFORM_OK TRUE)

    # Enable Objective C++ and ARC
    set (RT_COMPILER_FLAGS "${RT_COMPILER_FLAGS} -x objective-c++ -fobjc-arc")

    set (RT_INCLUDE_DIRS "")
    set (RT_LIB_DIRS "")
    set (RT_LIBS "-framework Cocoa")
endif ()
