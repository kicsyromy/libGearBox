#ifdef PLATFORM_MACOS
#include <catch.hpp>

#define private public
#include <libgearbox_http_macos_p.h>
#include <libgearbox_http_macos.cpp>

TEST_CASE("Test libgearbox_http_macos", "[http]")
{
}

#endif // PLATFORM_MACOS
