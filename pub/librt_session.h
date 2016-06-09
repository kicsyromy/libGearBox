#ifndef LIBRT_SESSION_H
#define LIBRT_SESSION_H

#include <vector>
#include <memory>

#include "librt_torrent.h"

namespace librt
{
    class SessionPrivate;

    class Session
    {
    public:
        static constexpr const char * DEFAULT_PATH = "/transmission/rpc";

    public:
        enum class Authentication
        {
            Required,
            None
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

        Session(const std::string &url,
                const std::string &path = DEFAULT_PATH,
                std::int8_t port = -1,
                Authentication authentication = Authentication::None,
                const std::string &username = "",
                const std::string &password = "");

        Session(std::string &&url,
                std::string &&path = DEFAULT_PATH,
                int8_t port = -1,
                Authentication authentication = Authentication::None,
                std::string &&username = "",
                std::string &&password = "");

    public:
        Statistics statistics() const;
        std::vector<librt::Torrent> torrents() const;

    public:
        std::string url() const;
        void setUrl(const std::string &url);
        void setUrl(std::string &&url);
        std::string path() const;
        void setPath(const std::string &path);
        void setPath(std::string &&path);
        bool authenticationRequired() const;
        void setAuthentication(Authentication authentication);
        std::string username() const;
        void setUsername(const std::string &username);
        void setUsername(std::string &&username);
        void setPassword(const std::string &password);
        void setPassword(std::string &&password);

    private:
        std::shared_ptr<SessionPrivate> priv_;
    };
}

#endif // LIBRT_SESSION_H
