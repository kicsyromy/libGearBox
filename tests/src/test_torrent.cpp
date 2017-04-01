#include <catch.hpp>

#define private public
#include <libgearbox_torrent.h>
#include <libgearbox_torrent.cpp>


TEST_CASE("Test libgearbox_torrent", "[torrent]")
{
    SECTION(("gearbox::Torrent::Torrent(gearbox::TorrentPrivate *)"))
    {
        auto priv = new TorrentPrivate();
        Torrent t(priv);

        REQUIRE((t.priv_.get() == priv));
    }
}
