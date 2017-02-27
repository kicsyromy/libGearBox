#include <catch.hpp>

#define private public
#include <libgearbox_folder_p.h>
#include <libgearbox_folder.h>
#include <libgearbox_folder.cpp>

TEST_CASE("Test librt_folder_p and librt_folder", "[folder]")
{
    SECTION("Folder::Folder(std::string &&)")
    {
        gearbox::Folder root("root");
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

    SECTION("gearbox::FolderPrivate::find(const std::string &)")
    {
        using namespace gearbox;

        Folder f("folder");
        auto &subfolders = f.priv_->subfolders_;
        subfolders.insert(std::make_pair("sub1", std::make_unique<Folder>("sub1")));
        subfolders.insert(std::make_pair("sub2", std::make_unique<Folder>("sub2")));
        subfolders.insert(std::make_pair("sub3", std::make_unique<Folder>("sub3")));

        auto subf = f.priv_->find("sub1");
        auto it = f.priv_->subfolders_.find("sub1");
        REQUIRE((subf == it->second.get()));
        subf = f.priv_->find("sub2");
        it = f.priv_->subfolders_.find("sub2");
        REQUIRE((subf == it->second.get()));
        subf = f.priv_->find("sub3");
        it = f.priv_->subfolders_.find("sub3");
        REQUIRE((subf == it->second.get()));
    }

    SECTION("gearbox::FolderPrivate::insert(std::string)")
    {
        using namespace gearbox;

        Folder f("folder");
        f.priv_->insert("sub1");
        f.priv_->insert("sub2");
        f.priv_->insert("sub3");

        auto &subfolders = f.priv_->subfolders_;
        REQUIRE((subfolders.size() == 3));
        auto it = subfolders.find("sub1");
        REQUIRE((it != subfolders.end()));
        it = subfolders.find("sub2");
        REQUIRE((it != subfolders.end()));
        it = subfolders.find("sub3");
        REQUIRE((it != subfolders.end()));
    }

    SECTION("gearbox::FolderPrivate::insert(gearbox::File &&)")
    {
        using namespace gearbox;

        Folder f("folder");
        f.priv_->insert(File {
            "file1",
            0,
            0,
            true,
            File::Priority::Normal
        });
        f.priv_->insert(File {
            "file2",
            0,
            0,
            true,
            File::Priority::Normal
        });
        f.priv_->insert(File {
            "file3",
            0,
            0,
            true,
            File::Priority::Normal
        });

        auto &files = f.priv_->files_;
        REQUIRE((files.size() == 3));
        auto it = files.find("file1");
        REQUIRE((it != files.end()));
        it = files.find("file2");
        REQUIRE((it != files.end()));
        it = files.find("file3");
        REQUIRE((it != files.end()));
    }

    SECTION("gearbox::FolderPrivate::addPath(const std::string &, std::size_t, std::uint64_t, std::uint64_t, bool, gearbox::File::Priority")
    {
        using namespace gearbox;

        Folder root("root");
        root.priv_->addPath("root/subl1/subl2/file.txt", 0, 0, 0, true, File::Priority::Normal);

        auto &subfoldersl1 = root.priv_->subfolders_;
        REQUIRE((subfoldersl1.size() == 1));
        auto subl1 = subfoldersl1.find("subl1");
        REQUIRE((subl1 != subfoldersl1.end()));
        auto &subfoldersl2 = subl1->second->priv_->subfolders_;
        auto subl2 = subfoldersl2.find("subl2");
        REQUIRE((subl2 != subfoldersl2.end()));

        auto &files = subl2->second->priv_->files_;
        REQUIRE((files.size() == 1));
        auto file = files.find("file.txt");
        REQUIRE((file != files.end()));
        REQUIRE((file->second->id_ == 0));
        REQUIRE((file->second->bytesCompleted_ == 0));
        REQUIRE((file->second->bytesTotal_ == 0));
        REQUIRE((file->second->wanted_ == true));
        REQUIRE((file->second->priority_ == File::Priority::Normal));
    }

    SECTION("gearbox::Folder::subfolders()")
    {
        using namespace gearbox;

        Folder f("folder");
        auto &subfolders = f.priv_->subfolders_;
        subfolders.insert(std::make_pair("sub1", std::make_unique<Folder>("sub1")));
        subfolders.insert(std::make_pair("sub2", std::make_unique<Folder>("sub2")));
        subfolders.insert(std::make_pair("sub3", std::make_unique<Folder>("sub3")));

        auto result = f.subfolders();
        REQUIRE((result.size() == 3));
        for (const Folder &folder: result)
        {
            REQUIRE((subfolders.find(folder.name()) != subfolders.end()));
        }
    }

    SECTION("gearbox::Folder::files()")
    {
        using namespace gearbox;

        Folder f("folder");
        auto &files = f.priv_->files_;
        files.insert(std::make_pair("file1", std::make_unique<File> (
            "file1",
            0,
            0,
            true,
            File::Priority::Normal
        )));
        files.insert(std::make_pair("file2", std::make_unique<File> (
            "file2",
            0,
            0,
            true,
            File::Priority::Normal
        )));
        files.insert(std::make_pair("file3", std::make_unique<File> (
            "file3",
            0,
            0,
            true,
            File::Priority::Normal
        )));

        auto result = f.files();
        REQUIRE((result.size() == 3));
        for (const File &file: result)
        {
            REQUIRE((files.find(file.name()) != files.end()));
        }
    }
}
