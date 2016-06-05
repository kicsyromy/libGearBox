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

    public:
        Session(const char *url,
                const char *path = DEFAULT_PATH,
                Authentication authentication = Authentication::None,
                const char *username = "",
                const char *password = "");
    public:
        std::vector<librt::Torrent> getTorrents();

    private:
        std::shared_ptr<SessionPrivate> priv_;
    };
}

#endif // LIBRT_SESSION_H
