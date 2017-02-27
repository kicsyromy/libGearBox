#include <catch.hpp>

#include <libgearbox_common_p.h>
#include <libgearbox_common.cpp>

TEST_CASE("Test librt_common_p", "[common]")
{
    gearbox::common::CaseInsensitiveCompare c;

    SECTION("bool CaseInsensitiveCompare::operator()(std::string, std::string)")
    {
        std::string strUpper = "STR";
        std::string strLower = "str";
        std::string strOther = "other";

        REQUIRE((!c(strUpper, strLower) && !c(strLower, strUpper)));
        REQUIRE(!(!c(strUpper, strOther) && !c(strOther, strUpper)));
        REQUIRE(!(!c(strLower, strOther) && !c(strOther, strLower)));
    }

    SECTION("bool CaseInsensitiveCompare::operator()(const char *, const char *)")
    {
        const char * strUpper = "STR";
        const char * strLower = "str";
        const char * strOther = "other";

        REQUIRE((!c(strUpper, strLower) && !c(strLower, strUpper)));
        REQUIRE(!(!c(strUpper, strOther) && !c(strOther, strUpper)));
        REQUIRE(!(!c(strLower, strOther) && !c(strOther, strLower)));
    }
}
