#ifndef LIBGEARBOX_SESSION_H
#define LIBGEARBOX_SESSION_H

#include <vector>
#include <memory>

#include <libgearbox_global.h>

#include <libgearbox_torrent.h>
#include <libgearbox_return_type.h>

namespace gearbox
{
    class SessionPrivate;

    class GEARBOX_API Session
    {
    public:
        static constexpr const char * DEFAULT_PATH { "/transmission/rpc" };
        static constexpr const std::int32_t PORT_AUTODETECT { -1 };

    public:
        enum class Authentication
        {
            Required,
            None
        };

        enum class SSLErrorHandling
        {
            Acknowledge,
            Ignore
        };

        struct Statistics
        {
            std::int32_t totalTorrentCount;
            std::int32_t activeTorrentCount;
            std::int32_t pausedTorrentCount;
            std::int32_t downloadSpeed;
            std::int32_t uploadSpeed;
        };

    public:
        Session();
        Session(Session &&other);
        Session &operator =(Session &&other);

        Session(const std::string &url,
                const std::string &path = DEFAULT_PATH,
                std::int32_t port = PORT_AUTODETECT,
                Authentication authentication = Authentication::None,
                const std::string &username = "",
                const std::string &password = "");

        Session(std::string &&url,
                std::string &&path = DEFAULT_PATH,
                std::int32_t port = -1,
                Authentication authentication = Authentication::None,
                std::string &&username = "",
                std::string &&password = "");

    public:
        ReturnType<Statistics> statistics() const;
        ReturnType<std::vector<gearbox::Torrent>> torrents() const;
        ReturnType<std::vector<std::int32_t>> recentlyRemoved() const;
        Error updateTorrentStats(std::vector<std::reference_wrapper<Torrent>> &torrents);

    public:
        const std::string &url() const;
        void setUrl(const std::string &url);
        void setUrl(std::string &&url);

        const std::string &path() const;
        void setPath(const std::string &path);
        void setPath(std::string &&path);

        std::int32_t port() const;
        void setPort(std::int32_t port);

        bool authenticationRequired() const;
        void setAuthentication(Authentication authentication);

        const std::string &username() const;
        void setUsername(const std::string &username);
        void setUsername(std::string &&username);

        const std::string &password() const;
        void setPassword(const std::string &password);
        void setPassword(std::string &&password);

        std::int32_t timeout() const;
        void setTimeout(int32_t value);

        void setSSLErrorHandling(SSLErrorHandling value);

    private:
        std::shared_ptr<SessionPrivate> priv_;

    private:
        DISABLE_COPY(Session)
    };
}

#endif // LIBGEARBOX_SESSION_H
