#include <catch.hpp>

#define private public
#include <libgearbox_file_p.h>
#include <libgearbox_file.h>
#include <libgearbox_file.cpp>

TEST_CASE("Test librt_file_p and librt_file", "[file]")
{
	librt::File f {
		"/libremotetransmission/test_file.txt",
		123456789,
		987654321,
		true,
		librt::File::Priority::Normal
	};

	SECTION("Test MIME Type")
	{
		REQUIRE((f.type().name == "text/plain"));
	}
}