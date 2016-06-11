#ifndef LIBRT_ERROR_H
#define LIBRT_ERROR_H

#include <string>

namespace librt
{
    class Error
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

            libRTInvalidSession
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
    };
}

#endif // LIBRT_ERROR_H
