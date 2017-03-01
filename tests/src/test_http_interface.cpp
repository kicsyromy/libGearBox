#include <catch.hpp>

#define private public
#include <libgearbox_http_interface_p.h>
#include <libgearbox_http_interface.cpp>

class TestHttpImplementation
{
private:
    using milliseconds_t = gearbox::http::milliseconds_t;
    using http_header_t = gearbox::http::header_t;
    using http_header_array_t = gearbox::http::header_array_t;
    using http_port_t = gearbox::http::port_t;
    using http_request_t = gearbox::http::RequestType;
    using http_ssl_error_handling_t = gearbox::http::SSLErrorHandling;
    using http_status_t = gearbox::http::Status;
    using http_error_t = gearbox::http::Error;
    using http_request_result_t = gearbox::http::RequestResult;

private:
    std::string userAgent_;
    std::string host_;
    http_port_t port_;
    std::string path_;
    bool authenticationRequired_;
    std::string username_;
    std::string password_;
    http_ssl_error_handling_t sslErrorHandling_;
    milliseconds_t timeout_;

public:
    explicit TestHttpImplementation(const std::string &userAgent) :
        userAgent_(userAgent),
        host_("http://domain.com"),
        port_(443),
        path_("/rpc/transmission"),
        authenticationRequired_(true),
        username_("user"),
        password_("pass"),
        sslErrorHandling_(http_ssl_error_handling_t::Aknowledge),
        timeout_(milliseconds_t { 1000 })
    {
    }

    TestHttpImplementation(TestHttpImplementation &&) noexcept(true) = default;
    TestHttpImplementation &operator =(TestHttpImplementation &&) noexcept(true) = default;

public:
    const std::string &host() const
    {
        return host_;
    }

    void setHost(const std::string &hostname)
    {
        host_ = hostname;
    }

    void setHost(std::string &&hostname)
    {
        host_ = std::move(hostname);
    }

    http_port_t port() const
    {
        return port_;
    }

    void setPort(http_port_t port)
    {
        port_ = port;
    }

    const std::string &path() const
    {
        return path_;
    }

    inline void setPath(const std::string &path)
    {
        path_ = path;
    }

    inline void setPath(std::string &&path)
    {
        path_ = std::move(path);
    }

    bool authenticationRequired() const
    {
        return authenticationRequired_;
    }

    void enableAuthentication()
    {
        authenticationRequired_ = true;
    }

    void disableAuthentication()
    {
        authenticationRequired_ = false;
    }

    const std::string &username() const
    {
        return username_;
    }

    void setUsername(const std::string &username)
    {
        username_ = username;
    }

    void setUsername(std::string &&username)
    {
        username_ = std::move(username);
    }

    const std::string &password() const
    {
        return password_;
    }

    void setPassword(const std::string &password)
    {
        password_ = password;
    }

    void setPassword(std::string &&password)
    {
        password_ = std::move(password);
    }

    void setSSLErrorHandling(http_ssl_error_handling_t value)
    {
        sslErrorHandling_ = value;
    }

    const milliseconds_t &timeout() const
    {
        return timeout_;
    }

    void setTimeout(milliseconds_t value)
    {
        timeout_ = value;
    }

public:
    struct Request
    {
        bool created = true;
    };

public:
    Request createRequest()
    {
        return {};
    }

private:
    DISABLE_COPY(TestHttpImplementation)
};

TEST_CASE("Test libgearbox_http_interface", "[http]")
{
    using HttpInterface = gearbox::http::Interface<TestHttpImplementation>;

    HttpInterface itf("UserAgent");

    SECTION("gearbox::http::Interface::Interface(const std::string &)")
    {
        REQUIRE((itf.implementation_.userAgent_ == "UserAgent"));
        REQUIRE((itf.implementation_.host_ == "http://domain.com"));
        REQUIRE((itf.implementation_.port_ == 443));
        REQUIRE((itf.implementation_.path_ == "/rpc/transmission"));
        REQUIRE((itf.implementation_.authenticationRequired_ == true));
        REQUIRE((itf.implementation_.username_ == "user"));
        REQUIRE((itf.implementation_.password_ == "pass"));
        REQUIRE((itf.implementation_.sslErrorHandling_ == gearbox::http::SSLErrorHandling::Aknowledge));
        REQUIRE((itf.implementation_.timeout_ == milliseconds_t { 1000 }));
    }

    SECTION(("gearbox::http::Interface::host() const"))
    {
        REQUIRE((itf.host() == "http://domain.com"));
    }

    SECTION(("gearbox::http::Interface::setHost(const std::string &)"))
    {
        const std::string copiable { "http://other-domain.com" };
        itf.setHost(copiable);
        REQUIRE((itf.implementation_.host_ == "http://other-domain.com"));
    }

    SECTION(("gearbox::http::Interface::setHost(const std::string &&)"))
    {
        std::string movable { "http://moved-domain.com" };
        itf.setHost(std::move(movable));
        REQUIRE((itf.implementation_.host_ == "http://moved-domain.com"));
        REQUIRE((movable.empty()));
    }

    SECTION(("gearbox::http::Interface::port() const"))
    {
        REQUIRE((itf.port() == 443));
    }

    SECTION(("gearbox::http::Interface::setPort(gearbox::http::port_t port)"))
    {
        itf.setPort(80);
        REQUIRE((itf.implementation_.port_ == 80));
    }
}
