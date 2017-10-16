#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#include "http_server.h"

inline int run_tests(int argc, char **argv)
{
    gearbox::test::startHttpServer();

    Catch::Session session;
    auto result = session.run(argc, argv);

    gearbox::test::stopHttpServer();

    return result < 0xff ? result : 0xff;
}

#ifdef PLATFORM_UWP
using namespace Platform;

int main(Array<String^>^ args)
{
    char program_name[] { "libgearboxtest" };
    char *program_name_ptr = &program_name[0];
    return run_tests(1, &program_name_ptr);
}
#else
extern "C" int main(int argc, char *argv[])
{
    return run_tests(argc, argv);
}
#endif
