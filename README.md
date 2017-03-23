# libGearBox
C++ library used for interacting with a remote [Transmission](https://transmissionbt.com/) server.

## Building
### Dependencies
The following combination of operating system and compiler are officially supported:
 * Windows (MinGW 4.9+, Microsoft Visual Studio 2015+)
 * Linux (GCC 4.9+, clang 3.8.1+) + cURL development libraries (libcurl4-openssl-dev on Ubuntu)
 * macOS (GCC 4.9+, clang 3.8.1+)

The project comes with all required (unless otherwise noted) dependencies as [git submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules). In adition to these [CMake](https://cmake.org/) is required to perform the actual build. Make sure that the cmake binary is in your `PATH`.

### Extra dependencies
Tests:
 * Python (min v3.4) libraries

Code coverage:
 * gcov
 * lcov
 * GCC/Clang/MinGW compiler
 
Documentation:
 * Doxygen

### Build steps
Clone the repository:
```bash
git clone https://github.com/kicsyromy/libGearBox.git
git submodule update --init
```
**Note:** Since CMake supports several Makefile formats, based on the target platform, I am not going to cover all of them here. Feel free to use the Visual Studio generator on Windows, or XCode generator on macOS. If a specific generator doesn't work for you please file a bug report.

**All platforms**
```
cd libGearBox
mkdir build
cd build
```

**Windows**
```
cmake .. -G "NMake Makefiles"
nmake
```

**Linux**
```
cmake .. -G "Unix Makefiles"
make -j $(grep -c ^processor /proc/cpuinfo)
```

**macOS**
```
cmake .. -G "Unix Makefiles"
make
```

The output of the build should be a static library called `libgearbox` with the appropriate extension (.lib on Windows, .a on Linux and macOS).

### Build options
libGearBox supports some [CMake options](https://cmake.org/cmake/help/v3.7/command/option.html) for a customized build. These are as follows:

`GEARBOX_STATIC_LIB` Build static library if set to ON otherwise build shared library. Defaults to ON.

`GEARBOX_DEBUG_LOGS` Build with additional debug logs. CAUTION: Do not use in production. Defaults to OFF.

`GEARBOX_ALLOW_UNSUPPORTED_COMPILER` Allow compilation using an unsupported compiler. Defaults to OFF.

`GEARBOX_ALLOW_UNSUPPORTED_PLATFORM` Allow compilation on an unsupported platform. Defaults to OFF.

`GEARBOX_ENABLE_TESTS` Build tests. Defaults to OFF.

`GEARBOX_TESTS_ENABLE_CODE_COVERAGE` Only available when building with tests. Enables code coverage generation using gcov/lcov.

`GEARBOX_BUILD_DOCUMENTATION` Build HTML documentation, also builds manpages when building under Linux. Defaults to OFF.

## Roadmap
TBD

## Contributing
Contributions are welcome, documentation, tests, code fixes etc. For a guideline on how contributions work on GitHub visit https://help.github.com/articles/working-with-forks/ and https://help.github.com/articles/proposing-changes-to-your-work-with-pull-requests/
