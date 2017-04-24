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

/*!
    \class gearbox::Error
    \brief Represents an error state consisting of an error code and a message

    Error is returned by any method call that makes an HTTP request. It can be
    stand-alone, or, accompanied by some data, returned by the server, wrapped
    in gearbox::ReturnType.
*/

#include "libgearbox_error.h"

/*!
    \enum gearbox::Error::Code

    \brief Error types including request failures, HTTP response errors, and library internal errors

    \var gearbox::Error::Ok
    \brief No error occured

    \var gearbox::Error::RequestConnectionFailure
    \brief There was a failure in establishing a connection to the server

    \var gearbox::Error::RequestRetry
    \brief The request needs to be sent once more

    \var gearbox::Error::RequestConnectionClosed
    \brief The connection was closed by the server

    \var gearbox::Error::RequestResponseInvalid
    \brief The response from the server was malformed

    \var gearbox::Error::RequestHeaderInvalid
    \brief One or more headers in the request were malformed. This could be due to incompatible server version

    \var gearbox::Error::RequestHeaderEmpty
    \brief The server sent an empty header

    \var gearbox::Error::RequestHostResolutionFailure
    \brief The requested host is not available

    \var gearbox::Error::RequestInternalError
    \brief An internal error in the library has occured

    \var gearbox::Error::RequestInvalidURL
    \brief The requested URL is invalid. The URL consists of the host and path

    \var gearbox::Error::RequestInvalid
    \brief The request is malformed

    \var gearbox::Error::RequestNetworkReceiveError
    \brief Could not receive any data from the server

    \var gearbox::Error::RequestNetworkSendFailure
    \brief Could not send any data to the server

    \var gearbox::Error::RequestOperationTimedOut
    \brief The request timed out

    \var gearbox::Error::RequestProxyResolutionFailure
    \brief Failed to connect to the system proxy

    \var gearbox::Error::RequestCookieDeclined
    \brief The supplied cookie was rejected by the server. With the current implementation this is not applicable

    \var gearbox::Error::RequestSSLConnectError
    \brief An attempt to establish a secure connection fails for reasons which cannot be expressed more specifically

    \var gearbox::Error::RequestSSLLocalCertificateError
    \brief A client certificate is required to authenticate an SSL connection

    \var gearbox::Error::RequestSSLRemoteCertificateError
    \brief The server supplied an untrusted or invalid certificate

    \var gearbox::Error::RequestSSLCacertError
    \brief The issuing Certificate Authority, that generated the server's certificate, is unknown

    \var gearbox::Error::RequestSSLRequestUnsecure
    \brief The application is moving from an SSL to an non-SSL connection because of a redirect

    \var gearbox::Error::RequestGenericSSLError
    \brief A place-holder error for a variety of SSL related errors

    \var gearbox::Error::RequestUnsupportedProtocol
    \brief The URL has an unsupported protocol

    \var gearbox::Error::HttpOK
    \brief Standard response for successful HTTP requests

    \var gearbox::Error::HttpCreated
    \brief The request has been fulfilled, resulting in the creation of a new resource

    \var gearbox::Error::HttpAccepted
    \brief The request has been accepted for processing, but the processing has not been completed

    \var gearbox::Error::HttpNonAuthoritativeInformation
    \brief The server is a transforming proxy that received a 200 OK from its origin, but is returning a modified version of the origin's response

    \var gearbox::Error::HttpNoContent
    \brief The server successfully processed the request and is not returning any content

    \var gearbox::Error::HttpResetContent
    \brief The server successfully processed the request, but is not returning any content

    \var gearbox::Error::HttpPartialContent
    \brief The server is delivering only part of the resource due to a range header sent by the client

    \var gearbox::Error::HttpIMUsed
    \brief The server has fulfilled a request for the resource, and the response is a representation of the result of one or more instance-manipulations applied to the current instance

    \var gearbox::Error::HttpMultipleChoices
    \brief Indicates multiple options for the resource from which the client may choose

    \var gearbox::Error::HttpMovedPermanently
    \brief This and all future requests should be directed to the given URI

    \var gearbox::Error::HttpFound
    \brief The response to the request can be found under another URI

    \var gearbox::Error::HttpSeeOther
    \brief The response to the request can be found under another URI

    \var gearbox::Error::HttpNotModified
    \brief Indicates that the resource has not been modified since the version specified by the request headers

    \var gearbox::Error::HttpUseProxy
    \brief The requested resource is available only through a proxy, the address for which is provided in the response

    \var gearbox::Error::HttpSwitchProxy
    \brief No longer used. Originally meant "Subsequent requests should use the specified proxy"

    \var gearbox::Error::HttpTemporaryRedirect
    \brief In this case, the request should be repeated with another URI; however, future requests should still use the original URI

    \var gearbox::Error::HttpPermanentRedirect
    \brief The request and all future requests should be repeated using another URI

    \var gearbox::Error::HttpBadRequest
    \brief The server cannot or will not process the request due to an apparent client error (e.g., malformed request syntax, too large size, invalid request message framing, or deceptive request routing)

    \var gearbox::Error::HttpUnauthorized
    \brief Similar to 403 Forbidden, but specifically for use when authentication is required and has failed or has not yet been provided

    \var gearbox::Error::HttpPaymentRequired
    \brief Reserved for future use

    \var gearbox::Error::HttpForbidden
    \brief The request was valid, but the server is refusing action

    \var gearbox::Error::HttpNotFound
    \brief The requested resource could not be found but may be available in the future

    \var gearbox::Error::HttpMethodNotAllowed
    \brief A request method is not supported for the requested resource; for example, a GET request on a form that requires data to be presented via POST

    \var gearbox::Error::HttpNotAcceptable
    \brief The requested resource is capable of generating only content not acceptable according to the Accept headers sent in the request

    \var gearbox::Error::HttpProxyAuthenticationRequired
    \brief The client must first authenticate itself with the proxy

    \var gearbox::Error::HttpRequestTimedOut
    \brief The client did not produce a request within the time that the server was prepared to wait. The client MAY repeat the request without modifications at any later time

    \var gearbox::Error::HttpConflict
    \brief Indicates that the request could not be processed because of conflict in the request; used by Transmission to signal an invalid session id

    \var gearbox::Error::HttpGone
    \brief Indicates that the resource requested is no longer available and will not be available again

    \var gearbox::Error::HttpLengthRequired
    \brief The request did not specify the length of its content

    \var gearbox::Error::HttpPreconditionFailed
    \brief The server does not meet one of the preconditions that the requester put on the request

    \var gearbox::Error::HttpPayloadTooLarge
    \brief The request is larger than the server is willing or able to process

    \var gearbox::Error::HttpURITooLong
    \brief The URI provided was too long for the server to process

    \var gearbox::Error::HttpUnsupportedMediaType
    \brief The request entity has a media type which the server or resource does not support

    \var gearbox::Error::HttpRangeNotSatisfiable
    \brief The client has asked for a portion of the file, but the server cannot supply that portion

    \var gearbox::Error::HttpExpectationFailed
    \brief The server cannot meet the requirements of the Expect request-header field

    \var gearbox::Error::HttpMisdirectedRequest
    \brief The server cannot meet the requirements of the Expect request-header field

    \var gearbox::Error::HttpUpgradeRequired
    \brief The client should switch to a different protocol such as TLS/1.0, given in the Upgrade header field

    \var gearbox::Error::HttpPreconditionRequired
    \brief The origin server requires the request to be conditional

    \var gearbox::Error::HttpTooManyRequests
    \brief The user has sent too many requests in a given amount of time

    \var gearbox::Error::HttpRequestHeaderFieldsTooLarge
    \brief The server is unwilling to process the request because either an individual header field, or all the header fields collectively, are too large

    \var gearbox::Error::HttpUnavailableForLegalReasons
    \brief A server operator has received a legal demand to deny access to a resource or to a set of resources that includes the requested resource

    \var gearbox::Error::HttpInternalServerError
    \brief A generic error message, given when an unexpected condition was encountered and no more specific message is suitable

    \var gearbox::Error::HttpNotImplemented
    \brief The server either does not recognize the request method, or it lacks the ability to fulfill the request

    \var gearbox::Error::HttpBadGateway
    \brief The server was acting as a gateway or proxy and received an invalid response from the upstream server

    \var gearbox::Error::HttpServiceUnavailable
    \brief The server is currently unavailable (because it is overloaded or down for maintenance)

    \var gearbox::Error::HttpGatewayTimedOut
    \brief The server was acting as a gateway or proxy and did not receive a timely response from the upstream server

    \var gearbox::Error::HttpHTTPVersionNotSupported
    \brief The server does not support the HTTP protocol version used in the request

    \var gearbox::Error::HttpVariantAlsoNegotiates
    \brief Transparent content negotiation for the request results in a circular reference

    \var gearbox::Error::HttpNotExtended
    \brief The server is unable to store the representation needed to complete the request

    \var gearbox::Error::HttpNetworkAuthenticationRequired
    \brief The client needs to authenticate to gain network access

    \var gearbox::Error::GearboxSessionInvalid
    \brief The gearbox::Session used by the gearbox::Torrent is invalid; this could mean that the session was destroyed before or during a request via an instance of gearbox::Torrent

    \var gearbox::Error::GearboxTorrentInvalid
    \brief The instance of gearbox::Torrent is invalid

    \var gearbox::Error::GearboxReserved_1
    \brief Reserved for future use

    \var gearbox::Error::GearboxReserved_2
    \brief Reserved for future use

    \var gearbox::Error::GearboxReserved_3
    \brief Reserved for future use

    \var gearbox::Error::GearboxReserved_4
    \brief Reserved for future use

    \var gearbox::Error::GearboxReserved_5
    \brief Reserved for future use

    \var gearbox::Error::GearboxReserved_6
    \brief Reserved for future use

    \var gearbox::Error::GearboxReserved_7
    \brief Reserved for future use

    \var gearbox::Error::GearboxReserved_8
    \brief Reserved for future use

    \var gearbox::Error::UnknownError
    \brief An unknown/unhandled error has occured
*/

using namespace gearbox;

/*!
    Constructs an instance of gearbox::Error
*/
Error::Error() : errorCode_(Error::Code::Ok), message_() {}

/*!
    Constructs an instance of gearbox::Error with the supplied gearbox::Error::Code and message
*/
Error::Error(Error::Code errorCode, std::string &&message)
  : errorCode_(errorCode), message_(message)
{
}

/*!
    Move constructor
*/
Error::Error(Error &&other)
  : errorCode_(other.errorCode_), message_(std::move(other.message_))
{
}

/*!
    Move asignment operator
*/
Error &Error::operator=(Error &&other)
{
    errorCode_ = other.errorCode_;
    message_ = std::move(other.message_);

    return *this;
}

/*!
    Initializes an existing instance of gearbox::Error from a pair of gearbox::Error::Code and a message
*/
Error &Error::operator=(std::pair<Error::Code, std::string> &&other)
{
    errorCode_ = other.first;
    message_ = std::move(other.second);

    return *this;
}

/*!
    Returns the gearbox::Error::Code from the instance of gearbox::Error
*/
Error::Code Error::errorCode() const { return errorCode_; }

/*!
    Returns a human readable message from the instance of gearbox::Error, that describes the error
*/
const std::string &Error::message() const { return message_; }

/*!
    \fn gearbox::Error::operator bool() const

    Implicit covnversion operator to check whether there is an error or not
*/
