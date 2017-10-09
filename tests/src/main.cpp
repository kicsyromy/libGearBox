#define CATCH_CONFIG_RUNNER

#include <catch.hpp>

#include "http_server.h"

//int main(int argc, char *argv[])
extern "C" int wmain (int argc, wchar_t * argv[], wchar_t * [])
{
    gearbox::test::startHttpServer();

    Catch::ConfigData config;
    config.showDurations = Catch::ShowDurations::Always;
    config.useColour = Catch::UseColour::Yes;
    config.outputFilename = "%debug";

    Catch::Session session;
    session.useConfigData(config);
    auto result = session.run(argc, argv);

    gearbox::test::stopHttpServer();

    return result < 0xff ? result : 0xff;
}
