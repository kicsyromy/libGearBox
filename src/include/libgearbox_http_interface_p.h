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

#ifndef LIBGEARBOX_HTTP_INTERFACE_H
#define LIBGEARBOX_HTTP_INTERFACE_H

#include <chrono>
#include <map>
#include <string>

#include <fmt/format.h>

#include "libgearbox_common_p.h"
#include "libgearbox_global.h"

#ifdef CHRONO_LITERALS
using namespace std::chrono_literals;
#endif

namespace gearbox
{
    namespace http
    {
        using header_t = std::pair<std::string, std::string>;
        using header_array_t =
            std::map<std::string, std::string, common::CaseInsensitiveCompare>;
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
                OK = 200,
                Created = 201,
                Accepted = 202,
                NonAuthoritativeInformation = 203,
                NoContent = 204,
                ResetContent = 205,
                PartialContent = 206,
                IMUsed = 226,

                /* REDIRECTION */
                MultipleChoices = 300,
                MovedPermanently = 301,
                Found = 302,
                SeeOther = 303,
                NotModified = 304,
                UseProxy = 305,
                SwitchProxy = 306,
                TemporaryRedirect = 307,
                PermanentRedirect = 308,

                /* CLIENT ERROR */
                BadRequest = 400,
                Unauthorized = 401,
                PaymentRequired = 402,
                Forbidden = 403,
                NotFound = 404,
                MethodNotAllowed = 405,
                NotAcceptable = 406,
                ProxyAuthenticationRequired = 407,
                RequestTimedOut = 408,
                Conflict = 409,
                Gone = 410,
                LengthRequired = 411,
                PreconditionFailed = 412,
                PayloadTooLarge = 413,
                URITooLong = 414,
                UnsupportedMediaType = 415,
                RangeNotSatisfiable = 416,
                ExpectationFailed = 417,
                MisdirectedRequest = 421,
                UpgradeRequired = 426,
                PreconditionRequired = 428,
                TooManyRequests = 429,
                RequestHeaderFieldsTooLarge = 431,
                UnavailableForLegalReasons = 451,

                /* SERVER ERROR */
                InternalServerError = 500,
                NotImplemented = 501,
                BadGateway = 502,
                ServiceUnavailable = 503,
                GatewayTimedOut = 504,
                HTTPVersionNotSupported = 505,
                VariantAlsoNegotiates = 506,
                NotExtended = 510,
                NetworkAuthenticationRequired = 511,

                /* Fallback */
                Unknown = 999
            };

            explicit Status(std::int32_t code);
            explicit Status(Status::Code value)
              : Status(static_cast<std::int32_t>(value))
            {
            }
            inline Status::Code code() const { return statusCode_; }
            inline const char *const name() const { return name_; }
            inline bool operator==(std::int32_t code)
            {
                return code == static_cast<std::int32_t>(statusCode_);
            }
            inline bool operator==(Status::Code code)
            {
                return code == statusCode_;
            }
            inline operator std::int32_t()
            {
                return static_cast<std::int32_t>(statusCode_);
            }
            inline operator std::string()
            {
                return fmt::format("'{} {}'", static_cast<int>(statusCode_),
                                   name_);
            }

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
            inline operator std::string()
            {
                return fmt::format("'{} {}'", static_cast<int>(errorCode),
                                   message);
            }
        };

        struct RequestResult
        {
            Status status;
            struct
            {
                header_array_t headers;
                std::string text;
            } response;
            double elapsed;
            Error error;
        };

        template <class Implementation> class Interface
        {
        public:
            explicit Interface(const std::string &userAgent)
              : implementation_(userAgent)
            {
            }
            Interface(Interface &&) noexcept(true) = default;
            Interface &operator=(Interface &&) noexcept(true) = default;

        public:
            inline const std::string &host() const
            {
                return implementation_.host();
            }
            inline void setHost(const std::string &hostname)
            {
                implementation_.setHost(hostname);
            }
            inline void setHost(std::string &&hostname)
            {
                implementation_.setHost(std::move(hostname));
            }

            inline port_t port() const { return implementation_.port(); }
            inline void setPort(port_t port) { implementation_.setPort(port); }

            inline const std::string &path() const
            {
                return implementation_.path();
            }
            inline void setPath(const std::string &path)
            {
                implementation_.setPath(path);
            }
            inline void setPath(std::string &&path)
            {
                implementation_.setPath(std::move(path));
            }

            inline bool authenticationRequired() const
            {
                return implementation_.authenticationRequired();
            }
            inline void enableAuthentication()
            {
                implementation_.enableAuthentication();
            }
            inline void disableAuthentication()
            {
                implementation_.disableAuthentication();
            }

            inline const std::string &username() const
            {
                return implementation_.username();
            }
            inline void setUsername(const std::string &username)
            {
                implementation_.setUsername(username);
            }
            inline void setUsername(std::string &&username)
            {
                implementation_.setUsername(std::move(username));
            }

            inline const std::string &password() const
            {
                return implementation_.password();
            }
            inline void setPassword(const std::string &password)
            {
                implementation_.setPassword(password);
            }
            inline void setPassword(std::string &&password)
            {
                implementation_.setPassword(std::move(password));
            }

            inline void setSSLErrorHandling(SSLErrorHandling value)
            {
                implementation_.setSSLErrorHandling(value);
            }

            inline const milliseconds_t &timeout() const
            {
                return implementation_.timeout();
            }
            inline void setTimeout(milliseconds_t value)
            {
                implementation_.setTimeout(value);
            }

        private:
            TYPE_HAS_METHOD(Implementation::Request, send, RequestResult());
            TYPE_HAS_METHOD(Implementation::Request,
                            setBody,
                            void(const std::string &));
            TYPE_HAS_METHOD(Implementation::Request,
                            setHeaders,
                            void(const header_array_t &));
            TYPE_HAS_METHOD(Implementation::Request,
                            setHeader,
                            void(const header_t &));

        public:
            inline typename Implementation::Request createRequest()
            {
                static_assert(has_send_v, "Missing "
                                          "'gearbox::http::RequestResult "
                                          "send()' method in Request type");
                static_assert(has_setBody_v, "Missing 'void setBody(const "
                                             "std::string &)' in Request type");
                static_assert(has_setHeaders_v, "Missing 'void "
                                                "setHeaders(const "
                                                "gearbox::http::header_array_t "
                                                "&)' in Request type");
                static_assert(has_setHeader_v, "Missing 'void setHeader(const "
                                               "gearbox::http::header_t &)' in "
                                               "Request type");
                return implementation_.createRequest();
            }

        private:
            Implementation implementation_;

        private:
            DISABLE_COPY(Interface)
        };
    }
}
#endif // LIBGEARBOX_HTTP_INTERFACE_H
