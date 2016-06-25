#include "librt_error.h"

using namespace librt;

Error::Error() :
    errorCode_(Error::Code::Ok),
    message_()
{
}

Error::Error(Error::Code errorCode, std::string &&message) :
    errorCode_(errorCode),
    message_(message)
{
}

Error::Error(Error &&other) :
    errorCode_(other.errorCode_),
    message_(std::move(other.message_))
{
}

Error &Error::operator =(Error &&other)
{
    errorCode_ = other.errorCode_;
    message_ = std::move(other.message_);

    return *this;
}

Error &Error::operator =(std::pair<Error::Code, std::string> &&other)
{
    errorCode_ = other.first;
    message_ = std::move(other.second);

    return *this;
}

Error::Code Error::errorCode() const
{
    return errorCode_;
}

const std::string &Error::message() const
{
    return message_;
}
