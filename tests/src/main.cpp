#define CATCH_CONFIG_RUNNER

#include <catch.hpp>

#include "http_server.h"

int main(int argc, char *argv[])
{
    gearbox::test::startHttpServer();

    auto result = Catch::Session().run(argc, argv);

    gearbox::test::stopHttpServer();

    return result < 0xff ? result : 0xff;
}
