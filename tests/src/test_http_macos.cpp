#ifdef PLATFORM_MACOS
#define private public
#include <libgearbox_http_macos_p.h>
#include <libgearbox_http_macos.cpp>

#include <catch.hpp>

TEST_CASE("Test libgearbox_http_macos", "[http]")
{
}

#endif // PLATFORM_LINUX
