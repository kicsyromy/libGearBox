#include <catch.hpp>

#define private public

#ifdef PLATFORM_LINUX
#include <libgearbox_http_linux_p.h>
#include <libgearbox_http_linux.cpp>

using HttpImplementation = gearbox::CUrlHttp;

TEST_CASE("Test libgearbox_http_linux_p", "[http]")
{
    SECTION(("<anonymous>::writeCallback(void *, std::size_t, std::size_t, std::string *data"))
    {
        std::string data { "some random data" };
        std::size_t size = data.size() / 2;
        std::size_t nmemb = 2;
        void *data_ptr = const_cast<char *>(data.data());
        std::string result;
        writeCallback(data_ptr, size, nmemb, &result);
        REQUIRE((result == data));

        size = data.size();
        nmemb = 1;
        result.clear();
        writeCallback(data_ptr, size, nmemb, &result);
        REQUIRE((result == data));
    }

    SECTION(("<anonymous>::headerCallback(void *, std::size_t, std::size_t, gearbox::http::header_array_t *)"))
    {
        std::string header { "HTTP/1.1 200 OK" };
        std::size_t size = header.size();
        std::size_t nmemb = 1;
        void *data_ptr = const_cast<char *>(header.data());
        gearbox::http::header_array_t result { { "dummy", "dummy" } };
        headerCallback(data_ptr, size, nmemb, &result);
        REQUIRE((result.empty()));

        header = { "key1: value1\r\n" };
        size = header.size();
        data_ptr = const_cast<char *>(header.data());
        headerCallback(data_ptr, size, nmemb, &result);

        header = { "key2: value2\r\n" };
        size = header.size();
        data_ptr = const_cast<char *>(header.data());
        headerCallback(data_ptr, size, nmemb, &result);

        header = { "key3: value3\r\n" };
        size = header.size();
        data_ptr = const_cast<char *>(header.data());
        headerCallback(data_ptr, size, nmemb, &result);

        header = { "key4: value4\r\n" };
        size = header.size();
        data_ptr = const_cast<char *>(header.data());
        headerCallback(data_ptr, size, nmemb, &result);

        REQUIRE((result.size() == 4));
        REQUIRE((result.at("key1") == "value1"));
        REQUIRE((result.at("key1") == "value1"));
        REQUIRE((result.at("key2") == "value2"));
        REQUIRE((result.at("key3") == "value3"));
        REQUIRE((result.at("key4") == "value4"));
    }
}
#endif // PLATFORM_LINUX

#ifdef PLATFORM_WINDOWS
#include <libgearbox_http_win_p.h>
#include <libgearbox_http_win.cpp>

using HttpImplementation = gearbox::WinHttp;

TEST_CASE("Test libgearbox_http_win_p", "[http]")
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
}

#endif // PLATFORM_WINDOWS

