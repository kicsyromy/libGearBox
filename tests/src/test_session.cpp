#include <catch.hpp>

#define private public
#include <libgearbox_session.h>
#include <libgearbox_session.cpp>
#include <libgearbox_torrent.h>
#include <libgearbox_torrent_p.h>

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

    {
        using gearbox::Session;

        Session test;
        SECTION(("gearbox::Session::Session()"))
        {
            REQUIRE((test.priv_ != nullptr));
            if (test.priv_ != nullptr)
            {
                auto &impl = test.priv_->http_;
                REQUIRE((test.priv_->sessionId_ == "dummy"));
                REQUIRE((impl.host() == ""));
                REQUIRE((impl.path() == Session::DEFAULT_PATH));
                REQUIRE((impl.port() == -1));
                REQUIRE((impl.authenticationRequired() == false));
                REQUIRE((impl.username() == ""));
                REQUIRE((impl.password() == ""));
                REQUIRE((impl.timeout() == http::milliseconds_t { DEFAULT_TIMEOUT }));
            }
        }

        Session testMove(std::move(test));
        SECTION(("gearbox::Session::Session(gearbox::Session &&)"))
        {
            REQUIRE((test.priv_ == nullptr));
            REQUIRE((testMove.priv_ != nullptr));
            if (test.priv_ != nullptr)
            {
                auto &impl = testMove.priv_->http_;
                REQUIRE((test.priv_->sessionId_ == "dummy"));
                REQUIRE((impl.host() == ""));
                REQUIRE((impl.path() == Session::DEFAULT_PATH));
                REQUIRE((impl.port() == -1));
                REQUIRE((impl.authenticationRequired() == false));
                REQUIRE((impl.username() == ""));
                REQUIRE((impl.password() == ""));
                REQUIRE((impl.timeout() == http::milliseconds_t { DEFAULT_TIMEOUT }));
            }
        }

        SECTION(("gearbox::Session::operator =(gearbox::Session &&)"))
        {
            test = std::move(testMove);

            REQUIRE((testMove.priv_ == nullptr));
            REQUIRE((test.priv_ != nullptr));
            if (test.priv_ != nullptr)
            {
                auto &impl = test.priv_->http_;
                REQUIRE((test.priv_->sessionId_ == "dummy"));
                REQUIRE((impl.host() == ""));
                REQUIRE((impl.path() == Session::DEFAULT_PATH));
                REQUIRE((impl.port() == -1));
                REQUIRE((impl.authenticationRequired() == false));
                REQUIRE((impl.username() == ""));
                REQUIRE((impl.password() == ""));
                REQUIRE((impl.timeout() == http::milliseconds_t { DEFAULT_TIMEOUT }));
            }
        }
    }

    SECTION(("gearbox::Session::Session(const std::string &, const std::string &, std::int32_t, bool, const std::string &, const std::string &)"))
    {
        using gearbox::Session;

        const std::string url = "test";
        const std::string path = "test";
        const std::int32_t port = -1;
        const std::string username = "username";
        const std::string password = "password";

        {
            Session test(
                url,
                path,
                port,
                Session::Authentication::Required,
                username,
                password
            );

            REQUIRE((test.priv_ != nullptr));
            if (test.priv_ != nullptr)
            {
                auto &impl = test.priv_->http_;
                REQUIRE((test.priv_->sessionId_ == "dummy"));
                REQUIRE((impl.host() == url));
                REQUIRE((impl.path() == path));
                REQUIRE((impl.port() == port));
                REQUIRE((impl.authenticationRequired() == true));
                REQUIRE((impl.username() == username));
                REQUIRE((impl.password() == password));
                REQUIRE((impl.timeout() == http::milliseconds_t{ DEFAULT_TIMEOUT }));
            }
        }

        {
            Session test(
                url,
                path,
                port,
                Session::Authentication::None,
                username,
                password
            );

            REQUIRE((test.priv_ != nullptr));
            if (test.priv_ != nullptr)
            {
                auto &impl = test.priv_->http_;
                REQUIRE((test.priv_->sessionId_ == "dummy"));
                REQUIRE((impl.host() == url));
                REQUIRE((impl.path() == path));
                REQUIRE((impl.port() == port));
                REQUIRE((impl.authenticationRequired() == false));
                REQUIRE((impl.username() == username));
                REQUIRE((impl.password() == password));
                REQUIRE((impl.timeout() == http::milliseconds_t{ DEFAULT_TIMEOUT }));
            }
        }
    }

    SECTION(("gearbox::Session::Session(std::string &&, std::string &&, std::int32_t, bool, std::string &&, std::string &&)"))
    {
        using gearbox::Session;

        const std::string url = "test";
        const std::string path = "test";
        const std::int32_t port = -1;
        const std::string username = "username";
        const std::string password = "password";

        {
            Session test(
                std::string(url),
                std::string(path),
                port,
                Session::Authentication::Required,
                std::string(username),
                std::string(password)
            );

            REQUIRE((test.priv_ != nullptr));
            if (test.priv_ != nullptr)
            {
                auto &impl = test.priv_->http_;
                REQUIRE((test.priv_->sessionId_ == "dummy"));
                REQUIRE((impl.host() == url));
                REQUIRE((impl.path() == path));
                REQUIRE((impl.port() == port));
                REQUIRE((impl.authenticationRequired() == true));
                REQUIRE((impl.username() == username));
                REQUIRE((impl.password() == password));
                REQUIRE((impl.timeout() == http::milliseconds_t{ DEFAULT_TIMEOUT }));
            }
        }

        {
            Session test(
                std::string(url),
                std::string(path),
                port,
                Session::Authentication::None,
                std::string(username),
                std::string(password)
            );

            REQUIRE((test.priv_ != nullptr));
            if (test.priv_ != nullptr)
            {
                auto &impl = test.priv_->http_;
                REQUIRE((test.priv_->sessionId_ == "dummy"));
                REQUIRE((impl.host() == url));
                REQUIRE((impl.path() == path));
                REQUIRE((impl.port() == port));
                REQUIRE((impl.authenticationRequired() == false));
                REQUIRE((impl.username() == username));
                REQUIRE((impl.password() == password));
                REQUIRE((impl.timeout() == http::milliseconds_t{ DEFAULT_TIMEOUT }));
            }
        }
    }

    {
        Session test(
            "http://localhost",
            gearbox::Session::DEFAULT_PATH,
            9999,
            Session::Authentication::Required,
            "username",
            "password"
        );

        SECTION(("gearbox::Session::statistics() const"))
        {
            auto stats = test.statistics();

            REQUIRE((!stats.error));
            REQUIRE((stats.value.activeTorrentCount == 42));
            REQUIRE((stats.value.downloadSpeed == 42));
            REQUIRE((stats.value.pausedTorrentCount == 42));
            REQUIRE((stats.value.totalTorrentCount == 42));
            REQUIRE((stats.value.uploadSpeed == 42));
        }

        SECTION(("gearbox::Session::torrents() const"))
        {
            auto torrents = test.torrents();

            REQUIRE((!torrents.error));
            REQUIRE((torrents.value.size() == 1));
            auto &t = torrents.value.at(0);
            REQUIRE((t.valid()));
            REQUIRE((t.id() == 0));
            REQUIRE((t.name() == "torrent"));
            REQUIRE((t.bytesDownloaded() == 1234));
            REQUIRE((t.percentDone() == 0.8));
            REQUIRE((t.uploadRatio() == 1.2));
            REQUIRE((t.bytesUploaded() == 1234567));
            REQUIRE((t.downloadSpeed() == 12345));
            REQUIRE((t.uploadSpeed() == 1234));
            REQUIRE((static_cast<int>(t.status()) == 4));
            REQUIRE((t.size() == 123456));
            REQUIRE((t.downloadDir() == "/path/to/downloads"));
            REQUIRE((t.eta() == 12345));
            REQUIRE((t.queuePosition() == 0));
        }

        SECTION(("gearbox::Session::recentlyRemoved() const"))
        {
        }

        SECTION(("gearbox::Session::updateTorrentStats(std::vector<std::reference_wrapper<gearbox::Torrent>> &)"))
        {
        }
    }

    {
        using gearbox::Session;

        const std::string url = "test";
        const std::string path = "test";
        const std::int32_t port = -1;
        const std::string username = "username";
        const std::string password = "password";

        Session test(
            url,
            path,
            port,
            Session::Authentication::Required,
            username,
            password
        );

        SECTION(("gearbox::Session::url() const"))
        {
            REQUIRE((test.host() == url));
        }

        SECTION(("gearbox::Session::path() const"))
        {
            REQUIRE((test.path() == path));
        }

        SECTION(("gearbox::Session::port() const"))
        {
            REQUIRE((test.port() == port));
        }

        SECTION(("gearbox::Session::authenticationRequired() const"))
        {
            REQUIRE((test.authenticationRequired()));
        }

        SECTION(("gearbox::Session::username() const"))
        {
            REQUIRE((test.username() == username));
        }

        SECTION(("gearbox::Session::password() const"))
        {
            REQUIRE((test.password() == password));
        }

        SECTION(("gearbox::Session::timeout() const"))
        {
            REQUIRE((test.timeout() == DEFAULT_TIMEOUT));
        }
    }
}
