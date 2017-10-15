#define CATCH_CONFIG_RUNNER

#include <catch.hpp>

#include "http_server.h"

#ifdef PLATFORM_UWP
using namespace Platform;
using namespace Windows::UI::Xaml;

int main(Array<String^>^ args)
{
    argc = args.size();
    argv = args.as_c_array();
#else
extern "C" int main(int argc, char *argv[])
{
#endif
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
