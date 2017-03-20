#ifdef PLATFORM_WINDOWS
#include <catch.hpp>

#include <iostream>

#define private public
#include <libgearbox_http_win_p.h>
#include <libgearbox_http_win.cpp>

#define CHECK_OPTION_STR(handle, option, value)            \
    do {                                                   \
        DWORD bufferLen;                                   \
        InternetQueryOption(                               \
            handle,                                        \
            option,                                        \
            nullptr,                                       \
            &bufferLen                                     \
        );                                                 \
        char *buffer = new char[bufferLen];                \
        InternetQueryOption(                               \
            handle,                                        \
            option,                                        \
            buffer,                                        \
            &bufferLen                                     \
        );                                                 \
        REQUIRE((strncmp(buffer, value, bufferLen) == 0)); \
        delete [] buffer;                                  \
    }                                                      \
    while(false)

#define CHECK_OPTION_NUM(handle, option, value)                          \
    do {                                                                 \
        DWORD size;                                                      \
        InternetQueryOption(                                             \
            handle,                                                      \
            option,                                                      \
            nullptr,                                                     \
            &size                                                        \
        );                                                               \
        unsigned long *val = static_cast<unsigned long *>(malloc(size)); \
        InternetQueryOption(                                             \
            handle,                                                      \
            option,                                                      \
            val,                                                         \
            &size                                                        \
        );                                                               \
        REQUIRE((val != nullptr));                                       \
        REQUIRE((*val == value));                                        \
        free(val);                                                       \
    }                                                                    \
    while(false)

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


    SECTION(("gearbox::WinHttp::WinHttp(const std::string &)"))
    {
        gearbox::WinHttp test("user-agent");
        CHECK_OPTION_STR(test.connection_, INTERNET_OPTION_USER_AGENT, "user-agent");
    }

    SECTION(("gearbox::WinHttp::setHost(std::string &&)"))
    {
        using gearbox::WinHttp;
        /* Test with no protocol in hostname */
        {
            WinHttp test("user-agent");
            test.setHost("some-domain.com");
            REQUIRE((test.hostname_ == "some-domain.com"));
            REQUIRE((test.port_ == HTTP_PORT));
            REQUIRE((test.connectionFlags_ == CONNECTION_FLAGS));
        }

        /* Test with short hostname (size < PREFIX_LEN)*/
        {
            WinHttp test("user-agent");
            test.setHost("short");
            REQUIRE((test.hostname_ == "short"));
            REQUIRE((test.port_ == HTTP_PORT));
            REQUIRE((test.connectionFlags_ == CONNECTION_FLAGS));
        }

        /* Test with http:// in hostname */
        {
            WinHttp test("user-agent");
            test.setHost("http://some-domain.com");
            REQUIRE((test.hostname_ == "some-domain.com"));
            REQUIRE((test.port_ == HTTP_PORT));
            REQUIRE((test.connectionFlags_ == CONNECTION_FLAGS));
        }

        /* Test with https:// in hostname */
        {
            WinHttp test("user-agent");
            test.setHost("https://some-domain.com");
            REQUIRE((test.hostname_ == "some-domain.com"));
            REQUIRE((test.port_ == HTTP_PORT_SSL));
            REQUIRE((test.connectionFlags_ == CONNECTION_FLAGS_SSL));
        }
    }

    SECTION(("gearbox::WinHttp::createRequest()"))
    {
        WinHttp test("user-agent");
        test.setHost("http://localhost");
        test.setPort(WinHttp::http_port_t { 9999 });
        test.setPath("/transmission/rpc");
        test.setTimeout(WinHttp::milliseconds_t { 2000 });
        test.setUsername("username");
        test.setPassword("password");

        auto request = test.createRequest();

        REQUIRE((request.session_ != nullptr));
        REQUIRE((request.path_ == "/transmission/rpc"));
        REQUIRE((request.requestType_ == WinHttp::http_request_t::GET));
        REQUIRE((request.connectionFlags_ ==
                 (CONNECTION_FLAGS |
                 INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
                 INTERNET_FLAG_IGNORE_CERT_DATE_INVALID)
        ));
        REQUIRE((request.headers_.empty()));
        REQUIRE((request.data_ == nullptr));
        REQUIRE((request.dataSize_ == 0));

        CHECK_OPTION_STR(request.session_.get(), INTERNET_OPTION_USER_AGENT, "user-agent");
        CHECK_OPTION_STR(request.session_.get(), INTERNET_OPTION_URL, "localhost");
        CHECK_OPTION_STR(request.session_.get(), INTERNET_OPTION_USERNAME, "username");
        CHECK_OPTION_STR(request.session_.get(), INTERNET_OPTION_PASSWORD, "password");
        CHECK_OPTION_NUM(request.session_.get(), INTERNET_OPTION_CONNECT_TIMEOUT, 2000);
        CHECK_OPTION_NUM(request.session_.get(), INTERNET_OPTION_SEND_TIMEOUT, 2000);
        CHECK_OPTION_NUM(request.session_.get(), INTERNET_OPTION_RECEIVE_TIMEOUT, 2000);
    }

    SECTION(("gearbox::WinHttp::Request::setBody(const std::string &)"))
    {
        DWORD_PTR rid;
        WinHttp::Request test(nullptr, "", 0, rid);
        REQUIRE((test.data_ == nullptr));
        REQUIRE((test.dataSize_ == 0));

        std::string body { "test data" };
        test.setBody(body);
        REQUIRE((test.data_ != nullptr));
        REQUIRE((strncmp(test.data_, body.c_str(), body.size()) == 0));
        REQUIRE((test.dataSize_ == body.size()));
        REQUIRE((test.requestType_ == WinHttp::http_request_t::POST));

        body = "new test data";
        test.setBody(body);
        REQUIRE((test.data_ != nullptr));
        REQUIRE((strncmp(test.data_, body.c_str(), body.size()) == 0));
        REQUIRE((test.dataSize_ == body.size()));
        REQUIRE((test.requestType_ == WinHttp::http_request_t::POST));
    }

    SECTION(("gearbox::WinHttp::Request::setHeader(s)(const std::string &)"))
    {
        DWORD_PTR rid;
        WinHttp::Request test(nullptr, "", 0, rid);

        WinHttp::http_header_array_t headers {
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

    SECTION(("gearbox::WinHttp::Request::send()"))
    {
        WinHttp test("user-agent");
        test.setHost("http://localhost");
        test.setPort(WinHttp::http_port_t { 9999 });
        test.setPath("/test_connection");

        auto request = test.createRequest();

        auto result = request.send();
        REQUIRE((result.error == 0));
        REQUIRE((result.status == WinHttp::http_status_t::OK));
        REQUIRE((result.response.text == "OK GET"));

        request.setBody("POST");
        result = request.send();
        REQUIRE((result.error == 0));
        REQUIRE((result.status == WinHttp::http_status_t::OK));
        REQUIRE((result.response.text == "OK POST"));
    }
}

#endif // PLATFORM_WINDOWS
