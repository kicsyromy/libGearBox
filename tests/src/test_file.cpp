#include <catch.hpp>

#include <cstring>

#define private public
#include <libgearbox_file_p.h>
#include <libgearbox_file.h>
#include <libgearbox_file.cpp>

TEST_CASE("Test librt_file_p and librt_file", "[file]")
{
	gearbox::File f {
		"/libremotetransmission/test_file.txt",
		123456789,
		987654321,
		true,
		gearbox::File::Priority::Normal
	};

	SECTION("Test MIME Type")
	{
		REQUIRE((strcmp(f.type().name,"text/plain") == 0));
	}
}