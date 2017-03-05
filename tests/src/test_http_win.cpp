#ifdef PLATFORM_WINDOWS
#include <catch.hpp>

#define private public
#include <libgearbox_http_win_p.h>
#include <libgearbox_http_win.cpp>

TEST_CASE("Test libgearbox_http_win", "[http]")
{
    SECTION(("<anonymous>::parseHeaders(char *, std::size_t)"))
    {
        using namespace std::literals::string_literals;

        /* Test standard HTTP header format */
        std::string headers =
            "HTTP/1.1 200 OK\0"
            "key1: value1\0"
            "key2: value2\0"
            "key3: value3\0"
            "key4: value4\0"s;
        auto data = const_cast<char *>(headers.data());

        auto result = parseHeaders(data, headers.size() + 1);
        REQUIRE((result.first == 200));
        REQUIRE((result.second.size() == 4));
        REQUIRE((result.second.at("key1") == "value1"));
        REQUIRE((result.second.at("key2") == "value2"));
        REQUIRE((result.second.at("key3") == "value3"));
        REQUIRE((result.second.at("key4") == "value4"));

        /* Test with empty HTTP header */
        headers.clear();
        data = const_cast<char *>(headers.data());
        result = parseHeaders(data, headers.size() + 1);
        REQUIRE((result.first == 0));
        REQUIRE((result.second.size() == 0));

        /* Test with missing response code */
        headers = 
            "key1: value1\0"
            "key2: value2\0"
            "key3: value3\0"
            "key4: value4\0"s;
        data = const_cast<char *>(headers.data());
        result = parseHeaders(data, headers.size() + 1);
        REQUIRE((result.first == 0));
        REQUIRE((result.second.size() == 3));
        REQUIRE((result.second.at("key2") == "value2"));
        REQUIRE((result.second.at("key3") == "value3"));
        REQUIRE((result.second.at("key4") == "value4"));

        /* Test with invalid response code */
        headers = 
            "HTTP/1.1 2\0"
            "key1: value1\0"
            "key2: value2\0"
            "key3: value3\0"
            "key4: value4\0"s;
        data = const_cast<char *>(headers.data());
        result = parseHeaders(data, headers.size() + 1);
        REQUIRE((result.first == 2));
        REQUIRE((result.second.size() == 3));
        REQUIRE((result.second.at("key2") == "value2"));
        REQUIRE((result.second.at("key3") == "value3"));
        REQUIRE((result.second.at("key4") == "value4"));

        /* Test with unexpected line terminator */
        headers = 
            "HTTP/1.1 200 OK"
            "key1: value1"
            "key2: value2"
            "key3: value3"
            "key4: value4"s;
        data = const_cast<char *>(headers.data());
        result = parseHeaders(data, headers.size() + 1);
        REQUIRE((result.first == 200));
        REQUIRE((result.second.size() == 0));
    }

    SECTION(("WinHttp::WinHttp(const std::string &)"))
    {
    }
}

#endif // PLATFORM_WINDOWS
