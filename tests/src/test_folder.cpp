#include <catch.hpp>

#define private public
#include <libgearbox_folder_p.h>
#include <libgearbox_folder.h>
#include <libgearbox_folder.cpp>

TEST_CASE("Test librt_folder_p and librt_folder", "[folder]")
{
	gearbox::Folder root("root");

	SECTION("Folder::Folder(std::string &&)")
	{
		REQUIRE((root.name() == "root"));
		REQUIRE((root.files().empty()));
		REQUIRE((root.subfolders().empty()));
	}

	SECTION("<anonymous>::rfind_char(char *, const std::size_t, char wanted)")
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

    SECTION("<anonymous>::rsplit(const std::string &, char)")
    {
        const std::string &str = { " a space separated list of stuff " };

        /* Test normal use-case */
        {
            const auto result = rsplit(str, ' ');
            REQUIRE((result.at(0) == ""));
            REQUIRE((result.at(1) == "stuff"));
            REQUIRE((result.at(2) == "of"));
            REQUIRE((result.at(3) == "list"));
            REQUIRE((result.at(4) == "separated"));
            REQUIRE((result.at(5) == "space"));
        }

        /* Test if separator not found */
        {
            const auto result = rsplit(str, '|');
            REQUIRE((result.empty()));
        }
    }
}
