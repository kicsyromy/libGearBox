#ifndef LIBGEARBOX_FILE_P_H
#define LIBGEARBOX_FILE_P_H

#include <sequential.h>

namespace gearbox
{
    struct FileAttr
    {
        ATTRIBUTE(std::uint64_t, bytesCompleted)
        ATTRIBUTE(std::uint64_t, length) /* size in bytes */
        ATTRIBUTE(std::string, name)
        INIT_ATTRIBUTES(bytesCompleted, length, name)
    };

    struct FileStat
    {
        ATTRIBUTE(std::uint64_t, bytesCompleted)
        ATTRIBUTE(bool, wanted) /* wether file should be downloaded or not */
        ATTRIBUTE(std::int8_t, priority)
        INIT_ATTRIBUTES(wanted, priority, bytesCompleted)
    };
}

#endif // LIBGEARBOX_FILE_P_H