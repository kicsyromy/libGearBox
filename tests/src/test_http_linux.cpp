#ifdef PLATFORM_LINUX

#include <catch.hpp>

#define private public
#include <libgearbox_http_linux_p.h>
#include <libgearbox_http_linux.cpp>

TEST_CASE("Test libgearbox_http_linux", "[http]")
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

    SECTION(("gearbox::CUrlHttp::CUrlHttp(std::string &)"))
    {
    }
}

#endif
