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

        header = "key1: value1\r\n";
        size = header.size();
        data_ptr = const_cast<char *>(header.data());
        headerCallback(data_ptr, size, nmemb, &result);

        header = "key2: value2\r\n";
        size = header.size();
        data_ptr = const_cast<char *>(header.data());
        headerCallback(data_ptr, size, nmemb, &result);

        header = "key3: value3\r\n";
        size = header.size();
        data_ptr = const_cast<char *>(header.data());
        headerCallback(data_ptr, size, nmemb, &result);

        header = "key4: value4\r\n";
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

    SECTION(("gearbox::CUrlHttp::CUrlHttp(const std::string &)"))
    {
        gearbox::CUrlHttp test("user-agent");
        REQUIRE((test.handle_ != nullptr));
    }

    SECTION(("gearbox::CUrlHttp::createRequest()"))
    {
        using gearbox::CUrlHttp;

        CUrlHttp test("user-agent");

        /* Test with no port, no path*/
        {
            test.setHost("http://domain.com");
            auto request = test.createRequest();
            REQUIRE((request.handle_ != nullptr));
            REQUIRE((request.handle_ != test.handle_));
            char *urlp = nullptr;
            curl_easy_getinfo(request.handle_, CURLINFO_EFFECTIVE_URL, &urlp);
            std::string url { urlp };
            REQUIRE((url == "http://domain.com/"));
        }

        /* Test with port, no path*/
        {
            test.setHost("http://domain.com");
            test.setPort(8080);
            auto request = test.createRequest();
            REQUIRE((request.handle_ != nullptr));
            REQUIRE((request.handle_ != test.handle_));
            char *urlp = nullptr;
            curl_easy_getinfo(request.handle_, CURLINFO_EFFECTIVE_URL, &urlp);
            std::string url { urlp };
            REQUIRE((url == "http://domain.com:8080/"));
        }

        /* Test with port and path*/
        {
            test.setHost("http://domain.com");
            test.setPort(8080);
            test.setPath("/some/path");
            auto request = test.createRequest();
            REQUIRE((request.handle_ != nullptr));
            REQUIRE((request.handle_ != test.handle_));
            char *urlp = nullptr;
            curl_easy_getinfo(request.handle_, CURLINFO_EFFECTIVE_URL, &urlp);
            std::string url { urlp };
            REQUIRE((url == "http://domain.com:8080/some/path"));
        }

        /* Test with no port and path*/
        {
            test.setHost("http://domain.com");
            test.setPort(-1);
            test.setPath("/some/path");
            auto request = test.createRequest();
            REQUIRE((request.handle_ != nullptr));
            REQUIRE((request.handle_ != test.handle_));
            char *urlp = nullptr;
            curl_easy_getinfo(request.handle_, CURLINFO_EFFECTIVE_URL, &urlp);
            std::string url { urlp };
            REQUIRE((url == "http://domain.com/some/path"));
        }
    }

    SECTION(("gearbox::CUrlHttp::Request::setBody(const std::string &)"))
    {
        /* Nothing to test with current implementation */
    }

    SECTION(("gearbox::CUrlHttp::Request::setHeader(s)(const std::string &)"))
    {
        using gearbox::CUrlHttp;

        auto handle = curl_easy_init();
        CUrlHttp::Request test(handle);

        CUrlHttp::http_header_array_t headers {
            { "key1", "value1" },
            { "key2", "value2" },
            { "key3", "value3" }
        };
        test.setHeaders(headers);
        REQUIRE((test.headers_ == headers));

        headers = {
            { "key1", "value4" },
            { "key2", "value5" },
            { "key3", "value6" }
        };
        test.setHeaders(headers);
        REQUIRE((test.headers_ == headers));

        headers["key4"] = "value7";
        test.setHeader({ "key4", "value7" });
        REQUIRE((test.headers_ == headers));

        headers["key4"] = "value8";
        test.setHeader({ "key4", "value8" });
        REQUIRE((test.headers_ == headers));
    }

    SECTION(("gearbox::CUrlHttp::Request::send()"))
    {
        using gearbox::CUrlHttp;

        CUrlHttp test("user-agent");
        test.setHost("http://localhost");
        test.setPort(CUrlHttp::http_port_t { 9999 });
        test.setPath("/test_connection");

        auto request = test.createRequest();

        auto result = request.send();
        REQUIRE((result.error == 0));
        REQUIRE((result.status == CUrlHttp::http_status_t::OK));
        REQUIRE((result.response.text == "OK GET"));

        request.setBody("POST");
        result = request.send();
        REQUIRE((result.error == 0));
        REQUIRE((result.status == CUrlHttp::http_status_t::OK));
        REQUIRE((result.response.text == "OK POST"));
    }
}

#endif // PLATFORM_LINUX
