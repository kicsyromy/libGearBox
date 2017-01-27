#ifndef LIBRT_HTTP_INTERFACE_H
#define LIBRT_HTTP_INTERFACE_H

#include <string>
#include <map>
#include <chrono>

#include <fmt/format.h>

#include "librt_global.h"
#include "librt_common_p.h"

#ifdef CHRONO_LITERALS
using namespace std::chrono_literals;
#endif

namespace librt
{
    namespace http
    {
        using header_t = std::pair<std::string, std::string>;
        using header_array_t = std::map<std::string, std::string, common::CaseInsensitiveCompare>;
        using milliseconds_t = decltype(std::chrono::milliseconds(0));
        using port_t = std::int32_t;

        enum class RequestType
        {
            GET,
            POST
        };

        enum class SSLErrorHandling
        {
            Aknowledge,
            Ignore
        };

        struct Status
        {
            enum Code
            {
                /* SUCCESS */
                OK                              = 200,
                Created                         = 201,
                Accepted                        = 202,
                NonAuthoritativeInformation     = 203,
                NoContent                       = 204,
                ResetContent                    = 205,
                PartialContent                  = 206,
                IMUsed                          = 226,

                /* REDIRECTION */
                MultipleChoices                 = 300,
                MovedPermanently                = 301,
                Found                           = 302,
                SeeOther                        = 303,
                NotModified                     = 304,
                UseProxy                        = 305,
                SwitchProxy                     = 306,
                TemporaryRedirect               = 307,
                PermanentRedirect               = 308,

                /* CLIENT ERROR */
                BadRequest                      = 400,
                Unauthorized                    = 401,
                PaymentRequired                 = 402,
                Forbidden                       = 403,
                NotFound                        = 404,
                MethodNotAllowed                = 405,
                NotAcceptable                   = 406,
                ProxyAuthenticationRequired     = 407,
                RequestTimedOut                 = 408,
                Conflict                        = 409,
                Gone                            = 410,
                LengthRequired                  = 411,
                PreconditionFailed              = 412,
                PayloadTooLarge                 = 413,
                URITooLong                      = 414,
                UnsupportedMediaType            = 415,
                RangeNotSatisfiable             = 416,
                ExpectationFailed               = 417,
                MisdirectedRequest              = 421,
                UpgradeRequired                 = 426,
                PreconditionRequired            = 428,
                TooManyRequests                 = 429,
                RequestHeaderFieldsTooLarge     = 431,
                UnavailableForLegalReasons      = 451,

                /* SERVER ERROR */
                InternalServerError             = 500,
                NotImplemented                  = 501,
                BadGateway                      = 502,
                ServiceUnavailable              = 503,
                GatewayTimedOut                 = 504,
                HTTPVersionNotSupported         = 505,
                VariantAlsoNegotiates           = 506,
                NotExtended                     = 510,
                NetworkAuthenticationRequired   = 511,

                /* Fallback */
                Unknown                         = 999
            };

            explicit Status(std::int32_t code);
            explicit Status(Status::Code value) : Status(static_cast<std::int32_t>(value)) {}
            inline Status::Code code() const { return statusCode_; }
            inline const char * const name() const { return name_; }
            inline bool operator ==(std::int32_t code) { return code == static_cast<std::int32_t>(statusCode_); }
            inline bool operator ==(Status::Code code) { return code == statusCode_; }
            inline operator std::int32_t() { return static_cast<std::int32_t>(statusCode_); }
            inline operator std::string() { return fmt::format("'{} {}'", static_cast<int>(statusCode_), name_); }

        private:
            Status::Code statusCode_;
            const char *name_;
        };

        struct Error
        {
            enum class Code
            {
                NoError = 0,
                ConnectionFailure,
                RetryRequest,
                ConnectionClosed,
                ResponseInvalid,
                HeaderInvalid,
                HeaderEmpty,
                HostResolutionFailure,
                InternalError,
                InvalidURL,
                RequestInvalid,
                NetworkReceiveError,
                NetworkSendFailure,
                OperationTimedOut,
                ProxyResolutionFailure,
                CookieDeclined,
                SSLConnectError,
                SSLLocalCertificateError,
                SSLRemoteCertificateError,
                SSLCacertError,
                SSLRequestUnsecure,
                GenericSSLError,
                UnsupportedProtocol,
                UnknownError = 999
            };
            Error::Code errorCode;
            const char *message;
            inline operator bool() { return errorCode != Error::Code::NoError; }
            inline operator std::string() { return fmt::format("'{} {}'", static_cast<int>(errorCode), message); }
        };

        struct RequestResult
        {
            Status status;
            struct {
                header_array_t headers;
                std::string text;
            } response;
            double elapsed;
            Error error;
        };

        template <class Implementation>
        class Interface
        {
        public:
            explicit Interface(const std::string &userAgent) : implementation_(userAgent) {}
            Interface(Interface &&) noexcept(true) = default;
            Interface &operator =(Interface &&) noexcept(true) = default;

        public:
            inline const std::string &host() const { return implementation_.host(); }
            inline void setHost(const std::string &hostname) { implementation_.setHost(hostname); }
            inline void setHost(std::string &&hostname) { implementation_.setHost(hostname); }

            inline port_t port() const { return implementation_.port(); }
            inline void setPort(port_t port) { implementation_.setPort(port); }

            inline const std::string &path() const { return implementation_.path(); }
            inline void setPath(const std::string &path) { implementation_.setPath(path); }
            inline void setPath(std::string &&path) { implementation_.setPath(path); }

            inline bool authenticationRequired() const { return implementation_.authenticationRequired(); }
            inline void enableAuthentication() { implementation_.enableAuthentication(); }
            inline void disableAuthentication() { implementation_.disableAuthentication(); }

            inline const std::string &username() const { return implementation_.username(); }
            inline void setUsername(const std::string &username) { implementation_.setUsername(username); }
            inline void setUsername(std::string &&username) { implementation_.setUsername(username); }

            inline const std::string &password() const { return implementation_.password(); }
            inline void setPassword(const std::string &password) { implementation_.setPassword(password); }
            inline void setPassword(std::string &&password) { implementation_.setPassword(password); }

            inline void setSSLErrorHandling(SSLErrorHandling value) {
                implementation_.setSSLErrorHandling(value);
            }

            inline const milliseconds_t &timeout() const { return implementation_.timeout(); }
            inline void setTimeout(milliseconds_t value) { implementation_.setTimeout(value); }

        public:
            inline typename Implementation::Request createRequest() {
                return implementation_.createRequest();
            }

        private:
            Implementation implementation_;

        private:
            DISABLE_COPY(Interface)
        };
    }
}
#endif // LIBRT_HTTP_INTERFACE_H
