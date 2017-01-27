#include <catch.hpp>

#define private public
#include <librt_file_p.h>
#include <librt_file.h>
#include <librt_file.cpp>

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