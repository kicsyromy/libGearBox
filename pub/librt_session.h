#ifndef LIBRT_SESSION_H
#define LIBRT_SESSION_H

#include <vector>

#include "librt_torrent.h"

namespace librt
{
    namespace session
    {
        std::vector<librt::Torrent> getTorrents();
    }
}

#endif // LIBRT_SESSION_H
