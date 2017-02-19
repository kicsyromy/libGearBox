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

#ifndef LIBGEARBOX_ERROR_H
#define LIBGEARBOX_ERROR_H

#include <string>

#include <libgearbox_global.h>

namespace gearbox
{
    class GEARBOX_API Error
    {
    public:
        enum class Code
        {
            Ok = 0,
            RequestConnectionFailure,
            RequestEmptyResponse,
            RequestHostResolutionFailure,
            RequestInternalError,
            RequestInvalidURLFormat,
            RequestNetworkReceiveError,
            RequestNetworkSendFailure,
            RequestOperationTimedout,
            RequestProxyResolutionFailure,
            RequestSSLConnectError,
            RequestSSLLocalCertificateError,
            RequestSSLRemoteCertificateError,
            RequestSSLCacertError,
            RequestGenericSSLError,
            RequestUnsupportedProtocol,

            TransmissionMethodNotAllowed,
            TransmissionUnknownError,

            libRTInvalidSession,
            libRTInvalidTorrent
        };

    public:
        Error();
        Error(Error::Code errorCode, std::string &&message);
        Error(Error &&other);
        Error &operator =(Error &&other);
        Error &operator =(std::pair<Error::Code, std::string> &&other);

    public:
        Error::Code errorCode() const;
        const std::string &message() const;

        inline operator bool() const { return errorCode_ != Error::Code::Ok; }

    private:
        Error::Code errorCode_;
        std::string message_;

    private:
        DISABLE_COPY(Error)
    };
}

#endif // LIBGEARBOX_ERROR_H
