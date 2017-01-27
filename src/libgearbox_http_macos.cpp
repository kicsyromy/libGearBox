#ifdef PLATFORM_MACOS
#if ! __has_feature(objc_arc)
#error "ARC is disabled"
#endif
#include "libgearbox_http_macos_p.h"

#import <Cocoa/Cocoa.h>

#include <iostream>
#include <chrono>
#include <cctype>

#include "libgearbox_vla_p.h"

namespace
{
    using namespace librt::http;

    Error fromNSURLError(NSError *error)
    {
        Error err { Error::Code::NoError, "" };

        if (error != nullptr)
        {
            switch ([error code])
            {
            case NSURLErrorFileDoesNotExist:
            case NSURLErrorFileIsDirectory:
            case NSURLErrorNoPermissionsToReadFile:
            case NSURLErrorCannotCreateFile:
            case NSURLErrorCannotOpenFile:
            case NSURLErrorCannotCloseFile:
            case NSURLErrorCannotWriteToFile:
            case NSURLErrorCannotRemoveFile:
            case NSURLErrorCannotMoveFile:
            case NSURLErrorDownloadDecodingFailedMidStream:
            case NSURLErrorDownloadDecodingFailedToComplete:
            case NSURLErrorAppTransportSecurityRequiresSecureConnection:
            case NSURLErrorBackgroundSessionInUseByAnotherProcess:
            case NSURLErrorBackgroundSessionRequiresSharedContainer:
            case NSURLErrorBackgroundSessionWasDisconnected:
            case NSURLErrorUnknown:
                err.errorCode = Error::Code::UnknownError;
                err.message = "The URL Loading system encountered an error that it cannot interpret";
                break;
            case NSURLErrorCancelled:
                err.errorCode = Error::Code::NetworkSendFailure;
                err.message = "An asynchronous load was canceled";
                break;
            case NSURLErrorBadURL:
                err.errorCode = Error::Code::InvalidURL;
                err.message = "The URL is sufficiently malformed that a URL request cannot be initiated";
                break;
            case NSURLErrorTimedOut:
                err.errorCode = Error::Code::OperationTimedOut;
                err.message = "Request timed oud";
                break;
            case NSURLErrorUnsupportedURL:
                err.errorCode = Error::Code::UnsupportedProtocol;
                err.message = "The properly formed URL cannot be handled by the framework";
                break;
            case NSURLErrorCannotFindHost:
                err.errorCode = Error::Code::HostResolutionFailure;
                err.message = "The host name for a URL cannot be resolved";
                break;
            case NSURLErrorCannotConnectToHost:
                err.errorCode = Error::Code::ConnectionFailure;
                err.message = "The attempt to connect to the host has failed";
                break;
            case NSURLErrorDataLengthExceedsMaximum:
                err.errorCode = Error::Code::NetworkSendFailure;
                err.message = "The length of the resource data exceeds the maximum allowed";
                break;
            case NSURLErrorNetworkConnectionLost:
                err.errorCode = Error::Code::ConnectionClosed;
                err.message = "The client or server connection is severed in the middle of an in-progress load";
                break;
            case NSURLErrorDNSLookupFailed:
                err.errorCode = Error::Code::HostResolutionFailure;
                err.message = "The host name for a URL cannot be resolved";
                break;
            case NSURLErrorHTTPTooManyRedirects:
                err.errorCode = Error::Code::InternalError;
                err.message = "A redirect loop is detected or the threshold for number of allowable redirects has been exceeded (currently 16)";
                break;
            case NSURLErrorResourceUnavailable:
                err.errorCode = Error::Code::InvalidURL;
                err.message = "The requested resource could not be retrieved";
                break;
            case NSURLErrorNotConnectedToInternet:
                err.errorCode = Error::Code::ConnectionFailure;
                err.message = "A network resource was requested, but an internet connection is not established and cannot be established automatically, either through a lack of connectivity, or by the user's choice not to make a network connection automatically";
                break;
            case NSURLErrorRedirectToNonExistentLocation:
                err.errorCode = Error::Code::ConnectionFailure;
                err.message = "A redirect is specified by way of server response code, but the server does not accompany this code with a redirect URL";
                break;
            case NSURLErrorBadServerResponse:
                err.errorCode = Error::Code::ResponseInvalid;
                err.message = "The URL Loading system received bad data from the server";
                break;
            case NSURLErrorUserCancelledAuthentication: // should never occur
                err.errorCode = Error::Code::InternalError;
                err.message = "An asynchronous request for authentication is cancelled by the user";
                break;
            case NSURLErrorUserAuthenticationRequired:
                err.errorCode = Error::Code::InternalError;
                err.message = "Authentication is required to access a resource";
                break;
            case NSURLErrorZeroByteResource:
                err.errorCode = Error::Code::ResponseInvalid;
                err.message = "The server reports that a URL has a non-zero content length, but terminates the network connection “gracefully” without sending any data";
                break;
            case NSURLErrorCannotDecodeRawData:
                err.errorCode = Error::Code::ResponseInvalid;
                err.message = "Content data received during an NSURLConnection request cannot be decoded for a known content encoding";
                break;
            case NSURLErrorCannotDecodeContentData:
                err.errorCode = Error::Code::ResponseInvalid;
                err.message = "Content data received during an NSURLConnection request has an unknown content encoding";
                break;
            case NSURLErrorCannotParseResponse:
                err.errorCode = Error::Code::ResponseInvalid;
                err.message = "The response to an NSURLConnection request cannot be parsed";
                break;
            case NSURLErrorInternationalRoamingOff:
                err.errorCode = Error::Code::ConnectionFailure;
                err.message = "The connection would require activating a data context while roaming, but international roaming is disabled";
                break;
            case NSURLErrorCallIsActive:
                err.errorCode = Error::Code::ConnectionFailure;
                err.message = "The connection is attempted while a phone call is active on a network that does not support simultaneous phone and data communication (EDGE or GPRS)";
                break;
            case NSURLErrorDataNotAllowed:
                err.errorCode = Error::Code::ConnectionFailure;
                err.message = "The cellular network disallows a connection";
                break;
            case NSURLErrorRequestBodyStreamExhausted:
                err.errorCode = Error::Code::InternalError;
                err.message = "A body stream is needed but the client does not provide one. This impacts clients on iOS that send a POST request using a body stream but do not implement the NSURLConnection delegate method connection:needNewBodyStream";
                break;
            case NSURLErrorSecureConnectionFailed:
                err.errorCode = Error::Code::SSLConnectError;
                err.message = "An attempt to establish a secure connection fails for reasons which cannot be expressed more specifically";
                break;
            case NSURLErrorServerCertificateHasBadDate:
                err.errorCode = Error::Code::SSLRemoteCertificateError;
                err.message = "The server certificate has a date which indicates it has expired, or is not yet valid";
                break;
            case NSURLErrorServerCertificateUntrusted:
                err.errorCode = Error::Code::SSLRemoteCertificateError;
                err.message = "The server certificate is signed by a root server which is not trusted";
                break;
            case NSURLErrorServerCertificateHasUnknownRoot:
                err.errorCode = Error::Code::SSLRemoteCertificateError;
                err.message = "The server certificate is not signed by any root server";
            case NSURLErrorServerCertificateNotYetValid:
                err.errorCode = Error::Code::SSLRemoteCertificateError;
                err.message = "The server certificate is not yet valid";
                break;
            case NSURLErrorClientCertificateRejected:
                err.errorCode = Error::Code::SSLRemoteCertificateError;
                err.message = "The server certificate is rejected";
                break;
            case NSURLErrorClientCertificateRequired:
                err.errorCode = Error::Code::SSLLocalCertificateError;
                err.message = "A client certificate is required to authenticate an SSL connection during an NSURLConnection request";
                break;
            case NSURLErrorCannotLoadFromNetwork:
                err.errorCode = Error::Code::ConnectionFailure;
                err.message = "A specific request to load an item only from the cache cannot be satisfied";
                break;
            }
        }

        return err;
    }

    struct Response
    {
        NSInteger status;
        NSDictionary *headers;
        NSData *body;
        NSError *error;
    };
    Response sendSynchronousRequest(NSURLSession *session,
                                    NSURLRequest *request,
                                    NSURLResponse __strong *&response,
                                    NSError __strong *&error)
    {
        NSError __block *err = nullptr;
        NSData __block *data;
        BOOL __block reqProcessed = FALSE;
        NSURLResponse __block *resp;

        void (^onRequestFinished)(NSData * _Nullable, NSURLResponse * _Nullable, NSError * _Nullable);
        onRequestFinished = ^(NSData * _Nullable _data,
                              NSURLResponse * _Nullable _response,
                              NSError * _Nullable _error)
        {
            resp = _response;
            err = _error;
            data = _data;
            reqProcessed = true;
        };
        [[session dataTaskWithRequest:request completionHandler:onRequestFinished] resume];

        while (!reqProcessed) {
            [NSThread sleepForTimeInterval:0];
        }

        response = resp;
        error = err;

        NSHTTPURLResponse *httpResponse = static_cast<NSHTTPURLResponse*>(response);
        return {
            [httpResponse statusCode],
            [httpResponse allHeaderFields],
            data,
            error
        };
    };
}

@interface NSURLRequest (DummyInterface)
+ (BOOL)allowsAnyHTTPSCertificateForHost:(NSString*)host;
+ (void)setAllowsAnyHTTPSCertificate:(BOOL)allow forHost:(NSString*)host;
@end

@interface librtCocoaHttpPrivate: NSObject { }

- (void) setUserAgent: (const std::string &)userAgent;
- (void) setHost: (const std::string &)hostname;
- (void) setPort: (int32_t)port;
- (void) setPath: (const std::string &)path;
- (void) enableAuthentication;
- (void) disableAuthentication;
- (void) setUsername: (const std::string &)username;
- (void) setPassword: (const std::string &)password;
- (void) setSSLErrorHandling: (bool)value;
- (void) setTimeout: (milliseconds_t)value;
- (librt::CocoaHttp::Request) createRequest;

@end

@implementation librtCocoaHttpPrivate
{
    NSString *userAgent_;
    NSString *hostname_;
    SInt32 port_;
    NSString *path_;
    BOOL authenticationEnabled_;
    NSString *username_;
    NSString *password_;
    BOOL handleSSLErrors_;
    UInt32 timeout_;
}

- (void) setUserAgent: (const std::string &)userAgent
{
    userAgent_ = [NSString stringWithUTF8String:userAgent.c_str()];
}

- (void) setHost: (const std::string &)hostname
{
    hostname_ = [NSString stringWithUTF8String:hostname.c_str()];
}

- (void) setPort: (std::int32_t)port
{
    port_ = port;
}

- (void) setPath: (const std::string &)path
{
    path_ = [NSString stringWithUTF8String:path.c_str()];
}

- (void) enableAuthentication
{
    authenticationEnabled_ = TRUE;
}

- (void) disableAuthentication
{
    authenticationEnabled_ = FALSE;
}

- (void) setUsername: (const std::string &)username
{
    authenticationEnabled_ = TRUE;
    username_ = [NSString stringWithUTF8String:username.c_str()];
}

- (void) setPassword: (const std::string &)password
{
    authenticationEnabled_ = TRUE;
    password_ = [NSString stringWithUTF8String:password.c_str()];
}

- (void) setSSLErrorHandling: (bool)value
{
    handleSSLErrors_ = value;
}

- (void) setTimeout: (milliseconds_t)value
{
    timeout_ = (UInt32)value.count();
}

- (librt::CocoaHttp::Request) createRequest
{
    NSString *urlString = nil;
    if (port_ > -1)
    {
        urlString = [@"" stringByAppendingFormat:@"%@:%d%@", hostname_, port_, path_];
    }
    else
    {
        urlString = [@"" stringByAppendingFormat:@"%@%@", hostname_, path_];
    }
    NSURL *url = [NSURL URLWithString:urlString];
    
    NSMutableURLRequest *request = [
        NSMutableURLRequest
        requestWithURL:url
        cachePolicy:NSURLRequestReloadIgnoringCacheData
        timeoutInterval:timeout_
    ];

    [request addValue:userAgent_ forHTTPHeaderField:@"User-Agent"];

    if (!handleSSLErrors_)
    {
        [NSURLRequest setAllowsAnyHTTPSCertificate:YES forHost:[url host]];
    }
    else
    {
        [NSURLRequest setAllowsAnyHTTPSCertificate:NO forHost:[url host]];
    }
    
    if (authenticationEnabled_)
    {
        NSData *loginData = [
            [@"" stringByAppendingFormat:@"%@:%@", username_, password_]
            dataUsingEncoding:NSUTF8StringEncoding
        ];
        NSString *encodedLoginData = [
            loginData
            base64EncodedStringWithOptions:
                NSDataBase64Encoding64CharacterLineLength |
                NSDataBase64EncodingEndLineWithLineFeed
        ];
        NSString *authorizationHeader = [@"Basic " stringByAppendingFormat:@"%@", encodedLoginData];
        [request addValue:authorizationHeader forHTTPHeaderField:@"Authorization"];
    }
    
    return { request };
}

@end

using namespace librt;

CocoaHttp::CocoaHttp(const std::string &userAgent) :
    hostname_(),
    port_(-1),
    path_("/"),
    authenticationEnabled_(false),
    authentication_(),
    sslErrorHandlingEnabled_(true),
    timeout_(),
    impl_([[librtCocoaHttpPrivate alloc] init])
{
    [impl_ setUserAgent:userAgent];
    [impl_ setPort:-1];
}

librt::CocoaHttp::~CocoaHttp()
{
}

const std::string &CocoaHttp::host() const
{
    return hostname_;
}

void CocoaHttp::setHost(const std::string &hostname)
{
    hostname_ = hostname;
    [impl_ setHost:hostname];
}

void CocoaHttp::setHost(std::string &&hostname)
{
    hostname_ = std::move(hostname);
    [impl_ setHost:hostname];
}

CocoaHttp::http_port_t CocoaHttp::port() const
{
    return port_;
}

void CocoaHttp::setPort(http_port_t port)
{
    port_ = port;
    [impl_ setPort:port];
}

const std::string &CocoaHttp::path() const
{
    return path_;
}

void CocoaHttp::setPath(const std::string &path)
{
    path_ = path;
    [impl_ setPath:path];
}

void CocoaHttp::setPath(std::string &&path)
{
    path_ = std::move(path);
    [impl_ setPath:path];
}

bool CocoaHttp::authenticationRequired() const
{
    return authenticationEnabled_;
}

void CocoaHttp::enableAuthentication()
{
    authenticationEnabled_ = true;
    [impl_ enableAuthentication];
}

void CocoaHttp::disableAuthentication()
{
    authenticationEnabled_ = false;
    [impl_ disableAuthentication];
}

const std::string &CocoaHttp::username() const
{
    return authentication_.username;
}

void CocoaHttp::setUsername(const std::string &username)
{
    authenticationEnabled_ = true;
    authentication_.username = username;
    [impl_ setUsername:username];
}

void CocoaHttp::setUsername(std::string &&username)
{
    authenticationEnabled_ = true;
    authentication_.username = std::move(username);
    [impl_ setUsername:username];
}

const std::string &CocoaHttp::password() const
{
    return authentication_.password;
}

void CocoaHttp::setPassword(const std::string &password)
{
    authenticationEnabled_ = true;
    authentication_.password = password;
    [impl_ setPassword:password];
}

void CocoaHttp::setPassword(std::string &&password)
{
    authenticationEnabled_ = true;
    authentication_.password = std::move(password);
    [impl_ setPassword:password];
}

void CocoaHttp::setSSLErrorHandling(http_ssl_error_handling_t value)
{
    sslErrorHandlingEnabled_ = (value == http_ssl_error_handling_t::Aknowledge);
    [impl_ setSSLErrorHandling:sslErrorHandlingEnabled_];
}

const milliseconds_t &CocoaHttp::timeout() const
{
    return timeout_;
}

void CocoaHttp::setTimeout(milliseconds_t value)
{
    timeout_ = value;
    [impl_ setTimeout:value];
}

CocoaHttp::Request::Request(NSMutableURLRequest *request) :
    request_(request),
    session_([NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]])
{
    [request_ setHTTPMethod:@"GET"];
}

CocoaHttp::Request::Request(CocoaHttp::Request &&) noexcept(true) = default;
CocoaHttp::Request &CocoaHttp::Request::operator =(CocoaHttp::Request &&) noexcept(true) = default;

CocoaHttp::Request::~Request() noexcept(true)
{
    [session_ invalidateAndCancel];
}

void CocoaHttp::Request::setBody(const std::string &data)
{
    if (data.empty())
    {
        [request_ setValue:@"0" forHTTPHeaderField:@"Content-Length"];
        [request_ setHTTPMethod:@"GET"];
        [request_ setHTTPBody:nil];
    }
    else
    {
        [request_ setHTTPMethod:@"POST"];
        NSString *postString = [NSString stringWithUTF8String:data.c_str()];
        NSData *postData = [postString dataUsingEncoding:NSUTF8StringEncoding];
        [request_
            setValue:[@"" stringByAppendingFormat:@"%lu", static_cast<unsigned long>([postData length])]
            forHTTPHeaderField:@"Content-Length"
        ];
        [request_ setHTTPBody:postData];
    }
}

void CocoaHttp::Request::setHeaders(const http_header_array_t &headers)
{
    for (const auto &header: headers)
    {
        NSString *key = [NSString stringWithUTF8String:header.first.c_str()];
        NSString *value= [NSString stringWithUTF8String:header.second.c_str()];
        [request_ setValue:value forHTTPHeaderField:key];
    }
}

void CocoaHttp::Request::setHeader(const http_header_t &header)
{
    NSString *key = [NSString stringWithUTF8String:header.first.c_str()];
    NSString *value= [NSString stringWithUTF8String:header.second.c_str()];
    [request_ setValue:value forHTTPHeaderField:key];
}

CocoaHttp::http_request_result_t CocoaHttp::Request::send()
{
    using namespace librt::http;
    std::int32_t httpStatus = librt::http::Status::Unknown;
    http_header_array_t responseHeaders;
    std::string text;
    http_error_t err = { Error::Code::InternalError, "An internal connection handle is invalid. "
        "This is most likely due to operating an a moved or otherwise invalidated "
        "instance of this object" };
    double elapsed = 0;
    
    if (request_ != nullptr)
    {
        NSError *error = nullptr;
        NSURLResponse *response = nullptr;

        const auto start = std::chrono::high_resolution_clock::now();
        auto r = sendSynchronousRequest(session_, request_, response, error);
        const auto end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / static_cast<double>(1000);

        httpStatus = static_cast<std::int32_t>(r.status);

        NSEnumerator *enumerator = [r.headers keyEnumerator];
        id key;
        while ((key = [enumerator nextObject]))
        {
            NSString *headerKey = static_cast<NSString *>(key);
            NSString *headerValue = static_cast<NSString *>([r.headers valueForKey:key]);
            responseHeaders[[headerKey UTF8String]] = [headerValue UTF8String];
        }

        text = { static_cast<const char *>([r.body bytes]), static_cast<std::size_t>([r.body length]) };
        err = fromNSURLError(r.error);
    }

    return {
        http_status_t(httpStatus),
        {
            responseHeaders,
            text
        },
        elapsed,
        err
    };
}

CocoaHttp::Request CocoaHttp::createRequest()
{
    return [impl_ createRequest];
}

#endif // PLATFORM_MACOS
