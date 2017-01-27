#ifdef PLATFORM_WINDOWS
#include "libgearbox_http_win_p.h"

#include <array>
#include <iostream>
#include <cctype>

#include <fmt/format.h>

#include "libgearbox_vla_p.h"
#include "libgearbox_logger_p.h"

using namespace librt;
using namespace std::chrono_literals;

namespace
{
    using namespace librt::http;

    constexpr std::array<const char *, 2> HTTP_METHOD {{ "GET", "POST" }};
    constexpr auto CONNECTION_FLAGS = INTERNET_FLAG_RELOAD | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE |
                                      INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;
    constexpr auto CONNECTION_FLAGS_SSL = INTERNET_FLAG_RELOAD | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE |
                                          INTERNET_FLAG_SECURE;
    constexpr port_t HTTP_PORT = INTERNET_DEFAULT_HTTP_PORT;
    constexpr port_t HTTP_PORT_SSL = INTERNET_DEFAULT_HTTPS_PORT;

    Error fromWinINetError(DWORD error)
    {
        Error err;

        switch (error)
        {
        default:
            err.errorCode = Error::Code::UnknownError;
            err.message = "An unkown error occured";
            break;
        case ERROR_HTTP_COOKIE_DECLINED:
            err.errorCode = Error::Code::CookieDeclined;
            err.message = "The HTTP cookie was declined by the server";
            break;
        case ERROR_HTTP_HEADER_NOT_FOUND:
            err.errorCode = Error::Code::HeaderEmpty;
            err.message = "The requested header has an empty value";
            break;
        case ERROR_HTTP_DOWNLEVEL_SERVER:
            err.errorCode = Error::Code::HeaderEmpty;
            err.message = "The server did not return any headers";
            break;
        case ERROR_HTTP_INVALID_HEADER:
            err.errorCode = Error::Code::HeaderInvalid;
            err.message = "The supplied header is invalid";
            break;
        case ERROR_HTTP_INVALID_SERVER_RESPONSE:
            err.errorCode = Error::Code::ResponseInvalid;
            err.message = "The server response could not be parsed";
            break;
        case ERROR_HTTP_NOT_REDIRECTED:
            err.errorCode = Error::Code::InternalError;
            err.message = "The HTTP request was not redirected";
            break;
        case ERROR_HTTP_REDIRECT_FAILED:
            err.errorCode = Error::Code::InternalError;
            err.message = "The redirection failed because either the "
                "scheme changed (for example, HTTP to FTP) "
                "or all attempts made to redirect failed "
                "(default is five attempts)";
            break;
        case ERROR_INTERNET_BAD_AUTO_PROXY_SCRIPT:
            err.errorCode = Error::Code::ProxyResolutionFailure;
            err.message = "There was an error in the automatic proxy configuration script";
            break;
        case ERROR_INTERNET_CANNOT_CONNECT:
            err.errorCode = Error::Code::ConnectionFailure;
            err.message = "The attempt to connect to the server failed.";
            break;
        case ERROR_INTERNET_CHG_POST_IS_NON_SECURE:
            err.errorCode = Error::Code::GenericSSLError;
            err.message = "The application is posting and attempting to change multiple lines of text on a server that is not secure";
            break;
        case ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED:
            err.errorCode = Error::Code::SSLLocalCertificateError;
            err.message = "The server is requesting client authentication";
            break;
        case ERROR_INTERNET_CONNECTION_ABORTED:
            err.errorCode = Error::Code::ConnectionClosed;
            err.message = "The connection with the server has been terminated";
            break;
        case ERROR_INTERNET_CONNECTION_RESET:
            err.errorCode = Error::Code::ConnectionClosed;
            err.message = "The connection with the server has been reset";
            break;
        case ERROR_INTERNET_DISCONNECTED:
            err.errorCode = Error::Code::ConnectionClosed;
            err.message = "The Internet connection has been lost";
            break;
        case ERROR_INTERNET_EXTENDED_ERROR:
        {
            err.errorCode = Error::Code::ConnectionFailure;
            DWORD inetError;
            char *buffer = nullptr;
            DWORD bufferLen;
            DWORD e = 0;
            do
            {
                InternetGetLastResponseInfoA(
                    &inetError,
                    buffer,
                    &bufferLen
                );
                e = GetLastError();
                if ((bufferLen > 0) && (buffer == nullptr))
                    buffer = new char[bufferLen];
            } while (e == ERROR_INSUFFICIENT_BUFFER);
            err.message = buffer; // == memory leak
            break;
        }
        case ERROR_INTERNET_FORCE_RETRY:
            err.errorCode = Error::Code::RetryRequest;
            err.message = "The request needs to be resent";
            break;
        case ERROR_INTERNET_HTTPS_HTTP_SUBMIT_REDIR:
            err.errorCode = Error::Code::SSLRequestUnsecure;
            err.message = "The data being submitted to an SSL connection is being redirected to a non-SSL connection";
            break;
        case ERROR_INTERNET_HTTPS_TO_HTTP_ON_REDIR:
            err.errorCode = Error::Code::SSLRequestUnsecure;
            err.message = "The application is moving from an SSL to an non-SSL connection because of a redirect";
            break;
        case ERROR_INTERNET_INCORRECT_FORMAT:
            err.errorCode = Error::Code::RequestInvalid;
            err.message = "The format of the request is invalid";
            break;
        case ERROR_INTERNET_INTERNAL_ERROR:
            err.errorCode = Error::Code::InternalError;
            err.message = "An internal error has occured";
            break;
        case ERROR_INTERNET_INVALID_CA:
            err.errorCode = Error::Code::SSLCacertError;
            err.message = "The issuing Certificate Authority, that generated the server's certificate, is unknown";
            break;
        case ERROR_INTERNET_INVALID_PROXY_REQUEST:
            err.errorCode = Error::Code::ProxyResolutionFailure;
            err.message = "The request to the proxy was invalid";
            break;
        case ERROR_INTERNET_INVALID_URL:
            err.errorCode = Error::Code::InvalidURL;
            err.message = "The URL is invalid";
            break;
        case ERROR_INTERNET_ITEM_NOT_FOUND:
            err.errorCode = Error::Code::InvalidURL;
            err.message = "The requested item could not be located";
            break;
        case ERROR_INTERNET_MIXED_SECURITY:
            err.errorCode = Error::Code::SSLRequestUnsecure;
            err.message = "The content is not entirely secure. "
                "Some of the content being viewed may have come from unsecured servers";
            break;
        case ERROR_INTERNET_NAME_NOT_RESOLVED:
            err.errorCode = Error::Code::HostResolutionFailure;
            err.message = "The server name could not be resolved";
            break;
        case ERROR_INTERNET_NO_DIRECT_ACCESS:
            err.errorCode = Error::Code::ConnectionFailure;
            err.message = "Direct network access cannot be made at this time";
            break;
        case ERROR_INTERNET_NOT_PROXY_REQUEST:
            err.errorCode = Error::Code::NetworkSendFailure;
            err.message = "The request cannot be made via a proxy";
            break;
        case ERROR_INTERNET_POST_IS_NON_SECURE:
            err.errorCode = Error::Code::SSLRequestUnsecure;
            err.message = "The application is posting data to a server that is not secure";
            break;
        case ERROR_INTERNET_PROTOCOL_NOT_FOUND:
            err.errorCode = Error::Code::UnsupportedProtocol;
            err.message = "The requested protocol could not be located";
            break;
        case ERROR_INTERNET_PROXY_SERVER_UNREACHABLE:
            err.errorCode = Error::Code::ProxyResolutionFailure;
            err.message = "The designated proxy server cannot be reached";
            break;
        case ERROR_INTERNET_SEC_CERT_CN_INVALID:
            err.errorCode = Error::Code::SSLRemoteCertificateError;
            err.message = "SSL certificate common name (host name field) is incorrect";
            break;
        case ERROR_INTERNET_SEC_CERT_DATE_INVALID:
            err.errorCode = Error::Code::SSLRemoteCertificateError;
            err.message = "SSL certificate date that was received from the server is expired";
            break;
        case ERROR_INTERNET_SEC_CERT_ERRORS:
            err.errorCode = Error::Code::SSLRemoteCertificateError;
            err.message = "The SSL certificate contains errors";
            break;
        case ERROR_INTERNET_SEC_CERT_REVOKED:
            err.errorCode = Error::Code::SSLRemoteCertificateError;
            err.message = "The SSL certificate was revoked";
            break;
        case ERROR_INTERNET_SEC_INVALID_CERT:
            err.errorCode = Error::Code::SSLRemoteCertificateError;
            err.message = "The SSL certificate is invalid";
            break;
        case ERROR_INTERNET_SECURITY_CHANNEL_ERROR:
            err.errorCode = Error::Code::GenericSSLError;
            err.message = "The application experienced an internal error loading the SSL libraries";
            break;
        case ERROR_INTERNET_SERVER_UNREACHABLE:
            err.errorCode = Error::Code::ConnectionFailure;
            err.message = "The website or server indicated is unreachable";
            break;
        case ERROR_INTERNET_UNRECOGNIZED_SCHEME:
            err.errorCode = Error::Code::InvalidURL;
            err.message = "The URL scheme could not be recognized, or is not supported";
            break;
        }

        return err;
    }

    std::pair<std::int32_t, header_array_t> parseHeaders(char *buffer, std::size_t length)
    {
        const char *key = nullptr;
        const char *statusStr = nullptr;
        header_array_t headers;

        std::size_t headersStart = 0;
        for (; headersStart < length - 1; ++headersStart)
        {
            if ((statusStr == nullptr) && (buffer[headersStart] == ' '))
            {
                statusStr = &buffer[headersStart + 1];
                headersStart += 4;
                if (headersStart < length - 1)
                {
                    buffer[headersStart++] = '\0';
                }
                else
                {
                    statusStr = nullptr;
                }
            }
            else if (buffer[headersStart] == '\0')
            {
                ++headersStart;
                break;
            }
        }
        key = &buffer[headersStart];
        for (std::size_t i = headersStart; i < length - 1; ++i)
        {
            if ((buffer[i] == ':') || (buffer[i] == '\0'))
            {
                buffer[i] = '\0';
                if (buffer[i + 1] != '\0')
                {
                    i += 2;
                    if (i < length)
                    {
                        auto it = headers.insert({ key, &buffer[i] });
                        i += it.first->second.size();
                        if (i < length - 1)
                            key = &buffer[i + 1];
                    }
                }
                else
                {
                    key = &buffer[i + 1];
                }
            }
        }

        return { std::strtol(statusStr, nullptr, 10), headers };
    }
}

WinHttp::WinHttp(const std::string &userAgent) :
    hostname_(),
    port_(-1),
    path_("/"),
    authenticationEnabled_(false),
    authentication_(),
    sslErrorHandlingEnabled_(true),
    timeout_(),
    connection_(nullptr),
    session_(nullptr),
    requestId_(0),
    connectionFlags_(-1)
{
    connection_ = InternetOpenA(
        userAgent.c_str(),
        INTERNET_OPEN_TYPE_PRECONFIG,
        nullptr,
        nullptr,
        0
    );

    if (connection_ == nullptr)
    {
        LOG_FATAL("Error {} while initiation connection.", static_cast<std::string>(fromWinINetError(GetLastError())));
    }
}

librt::WinHttp::WinHttp(WinHttp &&other) noexcept(true) :
    hostname_(std::move(other.hostname_)),
    port_(other.port_),
    path_(std::move(other.path_)),
    authenticationEnabled_(other.authenticationEnabled_),
    authentication_(std::move(other.authentication_)),
    sslErrorHandlingEnabled_(other.sslErrorHandlingEnabled_),
    timeout_(std::move_if_noexcept(other.timeout_)),
    connection_(other.connection_),
    session_(other.session_),
    requestId_(other.requestId_),
    connectionFlags_(other.connectionFlags_)
{
    other.connection_ = nullptr;
    other.session_ = nullptr;
}

librt::WinHttp::~WinHttp()
{
    if (session_ != nullptr)
    {
        InternetCloseHandle(session_);
        session_ = nullptr;
    }

    if (connection_ != nullptr)
    {
        InternetCloseHandle(connection_);
        connection_ = nullptr;
    }
}

const std::string &WinHttp::host() const
{
    return hostname_;
}

void WinHttp::setHost(const std::string &hostname)
{
    setHost(std::string(hostname));
}

void WinHttp::setHost(std::string &&hostname)
{
    closeSession();

    static constexpr std::size_t PREFIX_LEN { 9 };
    char prefix[PREFIX_LEN] = { '\0' };
    std::size_t flagCount = 0;
    std::size_t prefixEndIndex = 0;
    bool validPrefix = false;

    for (std::size_t i = 0; (i < PREFIX_LEN) && (flagCount < 3); ++i)
    {
        const auto character = hostname.at(i);
        switch (character)
        {
            case ':':
            {
                ++flagCount;
                break;
            }
            case '/':
            {
                if (flagCount > 0)
                    ++flagCount;
                else
                    flagCount = 0;
                break;
            }
            default:
            {
                flagCount = 0;
                break;
            }
        }
        prefix[i] = static_cast<char>(std::tolower(character));
        ++prefixEndIndex;
    }

    if (flagCount == 3)
    {
        if (std::strncmp(prefix, "https://", PREFIX_LEN) == 0)
        {
            connectionFlags_ = static_cast<int>(CONNECTION_FLAGS_SSL);
            if (port_ == -1)
                port_ = HTTP_PORT_SSL;
            validPrefix = true;
        }
        else if (std::strncmp(prefix, "http://", PREFIX_LEN) == 0)
        {
            validPrefix = true;
        }
    }

    if (connectionFlags_ == -1)
    {
        connectionFlags_ = static_cast<int>(CONNECTION_FLAGS);
        if (port_ == -1)
            port_ = HTTP_PORT;
    }

    hostname_ = validPrefix ? std::move(hostname.substr(prefixEndIndex)) : std::move(hostname);
}

WinHttp::http_port_t WinHttp::port() const
{
    return port_;
}

void WinHttp::setPort(http_port_t port)
{
    closeSession();
    port_ = port;
}

const std::string &WinHttp::path() const
{
    return path_;
}

void WinHttp::setPath(const std::string &path)
{
    path_ = path;
}

void WinHttp::setPath(std::string &&path)
{
    path_ = std::move(path);
}

bool WinHttp::authenticationRequired() const
{
    return authenticationEnabled_;
}

void WinHttp::enableAuthentication()
{
    closeSession();
    authenticationEnabled_ = true;
}

void WinHttp::disableAuthentication()
{
    closeSession();
    authenticationEnabled_ = false;
}

const std::string &WinHttp::username() const
{
    return authentication_.username;
}

void WinHttp::setUsername(const std::string &username)
{
    closeSession();
    authenticationEnabled_ = true;
    authentication_.username = username;
}

void WinHttp::setUsername(std::string &&username)
{
    closeSession();
    authenticationEnabled_ = true;
    authentication_.username = std::move(username);
}

const std::string &WinHttp::password() const
{
    return authentication_.password;
}

void WinHttp::setPassword(const std::string &password)
{
    closeSession();
    authenticationEnabled_ = true;
    authentication_.password = password;
}

void WinHttp::setPassword(std::string &&password)
{
    closeSession();
    authenticationEnabled_ = true;
    authentication_.password = std::move(password);
}

void WinHttp::setSSLErrorHandling(http_ssl_error_handling_t value)
{
    sslErrorHandlingEnabled_ = (value == http_ssl_error_handling_t::Aknowledge);
}

const milliseconds_t &WinHttp::timeout() const
{
    return timeout_;
}

void WinHttp::setTimeout(milliseconds_t value)
{
    closeSession();
    timeout_ = value;
}

WinHttp::Request::Request(HINTERNET session,
                                 const std::string &path,
                                 DWORD connectionFlags,
                                 DWORD_PTR &requestId) :
    session_(session),
    path_(path),
    requestType_(http_request_t::GET),
    connectionFlags_(connectionFlags),
    requestId_(requestId),
    headers_(),
    data_(nullptr),
    dataSize_(0)
{
}

WinHttp::Request::~Request()
{
    if (data_ != nullptr)
    {
        delete [] data_;
        data_ = nullptr;
    }
}

void WinHttp::Request::setBody(const std::string &data)
{
    requestType_ = http_request_t::POST;
    if (data_ != nullptr)
    {
        delete [] data_;
        data_ = nullptr;
    }

    data_ = _strdup(data.c_str());
    dataSize_ = static_cast<DWORD>(data.size());
}

void WinHttp::Request::setHeaders(const librt::WinHttp::http_header_array_t &headers)
{
    for (const auto &header: headers)
    {
        headers_[header.first] = header.second;
    }
}

void WinHttp::Request::setHeader(const librt::WinHttp::http_header_t &header)
{
    headers_[header.first] = header.second;
}

WinHttp::http_request_result_t WinHttp::Request::send()
{
    using namespace librt::http;
    std::int32_t httpStatus = Unknown;
    http_header_array_t responseHeaders;
    std::string text;
    http_error_t err = { Error::Code::NoError, "" };
    double elapsed = 0;

    auto request = HttpOpenRequestA(
        session_,
        HTTP_METHOD[static_cast<std::size_t>(requestType_)],
        path_.c_str(),
        "",
        nullptr,
        nullptr,
        connectionFlags_,
        requestId_++
    );

    if (request!= nullptr)
    {
        std::string headers;
        for (const auto &header : headers_)
        {
            headers += fmt::format("{}: {}\r\n", header.first, header.second);
        }
        headers += "\r\n\r\n";
        const auto start = std::chrono::high_resolution_clock::now();
        auto result = HttpSendRequestA(
            request,
            headers.c_str(),
            static_cast<DWORD>(headers.size()),
            data_,
            dataSize_
        );
        const auto end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / static_cast<double>(1000);
        if (result > 0)
        {
            DWORD error = 0;
            Array<char> headerBuffer;
            DWORD headerSize = 0;
            do
            {
                HttpQueryInfoA(
                    request,
                    HTTP_QUERY_RAW_HEADERS,
                    headerBuffer,
                    &headerSize,
                    nullptr
                );
                error = GetLastError();
                if ((headerSize > 0) && (headerBuffer == nullptr))
                    /* TODO: Check header size and allocate heap space if it exceeds */
                    /*       some well thought out limit                             */
                    headerBuffer = make_vla(char, headerSize);
            }
            while (error == ERROR_INSUFFICIENT_BUFFER);
            if (error > 0)
            {
                err = fromWinINetError(error);
            }
            else if ((headerSize > 0) && (headerBuffer != nullptr))
            {
                auto h = parseHeaders(headerBuffer, headerSize);
                httpStatus = h.first;
                responseHeaders = std::move(h.second);
                static constexpr std::size_t RESPONSE_LEN = 1024;
                DWORD bytesRead;
                char responseBuffer[RESPONSE_LEN] = { '\0' };
                int result = -1;

                do
                {
                    result = InternetReadFile(
                        request,
                        responseBuffer,
                        RESPONSE_LEN - 1,
                        &bytesRead
                    );
                    responseBuffer[bytesRead] = '\0';
                    text += responseBuffer;
                }
                while (result && bytesRead != 0);
            }
        }
        else
        {
            err = fromWinINetError(GetLastError());
        }

        InternetCloseHandle(request);
    }
    else
    {
        err = fromWinINetError(GetLastError());
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

WinHttp::Request WinHttp::createRequest()
{
    if (connection_ == nullptr)
    {
        LOG_FATAL("{}", "An internal connection handle is invalid. This is most likely due to "
                        "operating an a moved or otherwise invalidated instance of this object");
    }

    if (session_ == nullptr)
    {
        session_ = InternetConnectA(
            connection_,
            hostname_.c_str(),
            static_cast<INTERNET_PORT>(port_),
            authenticationEnabled_ ? authentication_.username.c_str() : nullptr,
            authenticationEnabled_ ? authentication_.password.c_str() : nullptr,
            INTERNET_SERVICE_HTTP,
            0,
            0
        );
        if (session_ != nullptr)
        {
            /* const */unsigned long timeout = static_cast<unsigned long>(timeout_.count());
            InternetSetOptionA(
                session_,
                INTERNET_OPTION_CONNECT_TIMEOUT,
                &timeout,
                sizeof(timeout)
            );
            InternetSetOptionA(
                session_,
                INTERNET_OPTION_SEND_TIMEOUT,
                &timeout,
                sizeof(timeout)
            );
            InternetSetOptionA(
                session_,
                INTERNET_OPTION_RECEIVE_TIMEOUT,
                &timeout,
                sizeof(timeout)
            );
        }
    }

    if (session_ == nullptr)
    {
        /* do some error handling */
    }

    return {
        session_,
        path_,
        !sslErrorHandlingEnabled_ ? static_cast<DWORD>(connectionFlags_) :
        static_cast<DWORD>(connectionFlags_ | INTERNET_FLAG_IGNORE_CERT_CN_INVALID
        | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID),
        requestId_
    };
}

void librt::WinHttp::closeSession()
{
    if (session_ != nullptr)
    {
        InternetCloseHandle(session_);
        session_ = nullptr;
    }
}

#endif // PLATFORM_WINDOWS

