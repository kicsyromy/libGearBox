#include <catch.hpp>

#define private public
#include <libgearbox_file_p.h>
#include <libgearbox_file.h>
#include <libgearbox_file.cpp>

#include <cstring>

TEST_CASE("Test librt_file_p and librt_file", "[file]")
{
    SECTION("<anonymous>::rfind_char(const char *, const std::size_t, char wanted)")
    {
        /* Test normal use-case */
        {
            char str[] = { "/some/long/path/" };
            const std::size_t length = std::strlen(str);

            auto result = rfind_char(str, length, '/');
            std::size_t index = static_cast<std::size_t>(result - str);
            REQUIRE((strcmp(result, "/") == 0));

            result = rfind_char(str, index, '/');
            index = static_cast<std::size_t>(result - str);
            REQUIRE((strcmp(result, "/path/") == 0));

            result = rfind_char(str, index, '/');
            index = static_cast<std::size_t>(result - str);
            REQUIRE((strcmp(result, "/long/path/") == 0));

            result = rfind_char(str, index, '/');
            index = static_cast<std::size_t>(result - str);
            REQUIRE((strcmp(result, str) == 0));
        }

        /* Test if separator not found */
        {
            char str[] = { "|use|different|separator" };
            const std::size_t length = std::strlen(str);
            auto result = rfind_char(str, length, ' ');
            REQUIRE((result == nullptr));
        }
    }

    SECTION("<anonymous>::mimeType(const std::string &)")
    {
        {
            std::string file { "file.txt" };
            REQUIRE((strcmp(mimeType(file).name, "text/plain") == 0));
            REQUIRE((mimeType(file).id == 479));
        }

        {
            std::string file { "file.missing" };
            REQUIRE((strcmp(mimeType(file).name, "") == 0));
            REQUIRE((mimeType(file).id == -1));
        }

        {
            std::string file { "file_without_ext" };
            REQUIRE((strcmp(mimeType(file).name, "") == 0));
            REQUIRE((mimeType(file).id == -1));
        }
    }

    gearbox::File f {
        "test_file.txt",
        0,
        0,
        true,
        gearbox::File::Priority::Normal
    };

    SECTION("gearbox::File::name() const")
    {
        REQUIRE((f.name() == "test_file.txt"));
    }

    SECTION("gearbox::File::bytesCompleted() const")
    {
        REQUIRE((f.bytesCompleted() == 0));
    }

    SECTION("gearbox::File::bytesTotal() const")
    {
        REQUIRE((f.bytesTotal() == 0));
    }

    SECTION("gearbox::File::wanted() const")
    {
        REQUIRE((f.wanted() == true));
    }

    SECTION("gearbox::File::priority() const")
    {
        REQUIRE((f.priority() == File::Priority::Normal));
    }

    SECTION("gearbox::File::type() const")
    {
        REQUIRE((strcmp(f.type().name, "text/plain") == 0));
        REQUIRE((f.type().id == 479));
    }
}
