/*
 * Copyright (c) 2016 Romeo Calota
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Author: Romeo Calota
 */

#include "libgearbox_error.h"

using namespace gearbox;

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
