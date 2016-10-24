#include "librt_file.h"

using namespace librt;

File::File(std::string &&name,
                    std::uint64_t bytesCompleted,
                    std::uint64_t bytesTotal,
                    bool wanted,
                    File::Priority priority,
                    MIMEType type) :
    id_(0),
    name_(std::move(name)),
    bytesCompleted_(bytesCompleted),
    bytesTotal_(bytesTotal),
    wanted_(wanted),
    priority_(priority),
    type_(type)
{
}

const std::string &File::name() const
{
    return name_;
}

uint64_t File::bytesCompleted() const
{
    return bytesCompleted_;
}

uint64_t File::bytesTotal() const
{
    return bytesTotal_;
}

bool File::wanted() const
{
    return wanted_;
}

File::Priority File::priority() const
{
    return priority_;
}

File::MIMEType File::type() const
{
    return type_;
}
