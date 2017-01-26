#include <catch.hpp>

#include <librt_common_p.h>

TEST_CASE("Test common functionaly used in other modules", "[common]")
{
	librt::common::CaseInsensitiveCompare c;

	SECTION("Comparison operator for std::string")
	{
		std::string strUpper = "STR";
		std::string strLower = "str";
		std::string strOther = "other";

		REQUIRE((!c(strUpper, strLower) && !c(strLower, strUpper)));
		REQUIRE(!(!c(strUpper, strOther) && !c(strOther, strUpper)));
		REQUIRE(!(!c(strLower, strOther) && !c(strOther, strLower)));
	}

	SECTION("Comparison operator for const char *")
	{
		const char * strUpper = "STR";
		const char * strLower = "str";
		const char * strOther = "other";

		REQUIRE((!c(strUpper, strLower) && !c(strLower, strUpper)));
		REQUIRE(!(!c(strUpper, strOther) && !c(strOther, strUpper)));
		REQUIRE(!(!c(strLower, strOther) && !c(strOther, strLower)));
	}
}