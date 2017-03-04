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

TEST_CASE("Test libgearbox_http_win_p", "[http]")
{
    SECTION(("<anonymous>::parseHeaders(char *, std::size_t)"))
    {
        char headers[] = {
            "HTTP/1.1 200 OK\0"
            "Server: Microsoft-IIS/5.0\0"
            "Date: Thu, 13 Jul 2000 05:46:53 GMT\0"
            "Content-Length: 2291\0"
            "Content-Type: text / html\0"
            "Set-Cookie: ASPSESSIONIDQQGGGNCG = LKLDFFKCINFLDMFHCBCBMFLJ; path=/\0"
            "Cache-control: private\0"
        };

        auto result = parseHeaders(headers, sizeof(headers));
    }
}

#endif // PLATFORM_WINDOWS

