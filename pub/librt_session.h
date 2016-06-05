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
        Session(const char *url,
                const char *path = DEFAULT_PATH,
                Authentication authentication = Authentication::None,
                const char *username = "",
                const char *password = "");
    public:
        Statistics statistics() const;
        std::vector<librt::Torrent> torrents() const;

    private:
        std::shared_ptr<SessionPrivate> priv_;
    };
}

#endif // LIBRT_SESSION_H
