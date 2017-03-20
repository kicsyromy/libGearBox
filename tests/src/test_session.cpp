#include <catch.hpp>

#define private public
#include <libgearbox_session.h>
#include <libgearbox_session.cpp>

TEST_CASE("Test libgearbox_session", "[session]")
{
    SECTION(("gearbox::SessionPrivate::SessionPrivate(const std::string &, const std::string &, std::int32_t, bool, const std::string &, const std::string &)"))
    {
        const std::string url = "test";
        const std::string path = "test";
        const std::int32_t port = -1;
        const bool auth = true;
        const std::string username = "username";
        const std::string password = "password";

        SessionPrivate test(
            url,
            path,
            port,
            auth,
            username,
            password
        );

        REQUIRE((test.sessionId_ == "dummy"));
        REQUIRE((test.http_.host() == url));
        REQUIRE((test.http_.path() == path));
        REQUIRE((test.http_.port() == port));
        REQUIRE((test.http_.authenticationRequired() == auth));
        REQUIRE((test.http_.username() == username));
        REQUIRE((test.http_.password() == password));
        REQUIRE((test.http_.timeout() == http::milliseconds_t { DEFAULT_TIMEOUT }));
    }

    SECTION(("gearbox::SessionPrivate::SessionPrivate(std::string &&, std::string &&, std::int32_t, bool, std::string &&, std::string &&)"))
    {
        const std::string url = "test";
        const std::string path = "test";
        const std::int32_t port = -1;
        const bool auth = true;
        const std::string username = "username";
        const std::string password = "password";

        SessionPrivate test(
            std::string(url),
            std::string(path),
            port,
            auth,
            std::string(username),
            std::string(password)
        );

        REQUIRE((test.sessionId_ == "dummy"));
        REQUIRE((test.http_.host() == url));
        REQUIRE((test.http_.path() == path));
        REQUIRE((test.http_.port() == port));
        REQUIRE((test.http_.authenticationRequired() == auth));
        REQUIRE((test.http_.username() == username));
        REQUIRE((test.http_.password() == password));
        REQUIRE((test.http_.timeout() == http::milliseconds_t { DEFAULT_TIMEOUT }));
    }

    SECTION(("gearbox::SessionPrivate::sendRequest(const std::string &, nlohmann::json)"))
    {
        SessionPrivate test(
            "http://localhost",
            gearbox::Session::DEFAULT_PATH,
            9999,
            true,
            "username",
            "password"
        );

        auto response = test.sendRequest("test_send_request", {});
        REQUIRE((response.get_tag() == SESSION_TAG));
        REQUIRE((response.get_result() == "success"));
        REQUIRE((response.get_arguments().value<int>("args", -1) == 0));
    }
}
