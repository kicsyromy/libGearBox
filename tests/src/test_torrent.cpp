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

    SECTION(("gearbox::Torrent::valid()"))
    {
        auto priv = new TorrentPrivate();
        Torrent t(priv);
        REQUIRE((t.valid()));
        t.priv_.reset(nullptr);
        REQUIRE((!t.valid()));
    }

    {
        auto priv = new TorrentPrivate();
        std::get<0> (priv->attributes) = 0;                       /* id */
        std::get<1> (priv->attributes).set_value("name");         /* name */
        std::get<2> (priv->attributes) = 1;                       /* bytes downloaded */
        std::get<3> (priv->attributes) = 2;                       /* percent done */
        std::get<4> (priv->attributes) = 3;                       /* upload ration */
        std::get<5> (priv->attributes) = 7;                       /* bytes uploaded */
        std::get<6> (priv->attributes) = 5;                       /* download speed */
        std::get<7> (priv->attributes) = 6;                       /* upload speed */
        std::get<8> (priv->attributes) = 4;                       /* status */
        std::get<9> (priv->attributes) = 8;                       /* size */
        std::get<10>(priv->attributes).set_value("download-dir"); /* download dir */
        std::get<11>(priv->attributes) = 9;                       /* eta */
        std::get<12>(priv->attributes) = 10;                      /* queue position */
        Torrent t(priv);

        SECTION(("gearbox::Torrent::id() const"))
        {
            REQUIRE((t.id() == 0));
        }

        SECTION(("gearbox::Torrent::name() const"))
        {
            REQUIRE((t.name() == "name"));
        }

        SECTION(("gearbox::Torrent::bytesDownloaded() const"))
        {
            REQUIRE((t.bytesDownloaded() == 1));
        }

        SECTION(("gearbox::Torrent::percentDone() const"))
        {
            REQUIRE((t.percentDone() == 2));
        }

        SECTION(("gearbox::Torrent::uploadRatio() const"))
        {
            REQUIRE((t.uploadRatio() == 3));
        }

        SECTION(("gearbox::Torrent::bytesUploaded() const"))
        {
            REQUIRE((t.bytesUploaded() == 7));
        }

        SECTION(("gearbox::Torrent::downloadSpeed() const"))
        {
            REQUIRE((t.downloadSpeed() == 5));
        }

        SECTION(("gearbox::Torrent::uploadSpeed() const"))
        {
            REQUIRE((t.uploadSpeed() == 6));
        }

        SECTION(("gearbox::Torrent::status() const"))
        {
            REQUIRE((static_cast<int>(t.status()) == 4));
        }

        SECTION(("gearbox::Torrent::size() const"))
        {
            REQUIRE((t.size() == 8));
        }

        SECTION(("gearbox::Torrent::downloadDir() const"))
        {
            REQUIRE((t.downloadDir() == "download-dir"));
        }

        SECTION(("gearbox::Torrent::eta() const"))
        {
            REQUIRE((t.eta() == 9));
        }

        SECTION(("gearbox::Torrent::queuePosition() const"))
        {
            REQUIRE((t.queuePosition() == 10));
        }
    }
}
