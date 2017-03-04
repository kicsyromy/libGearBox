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

#ifdef PLATFORM_LINUX
#include "libgearbox_http_linux_p.h"

#include <iostream>
#include <cctype>

#include "libgearbox_vla_p.h"
#include "libgearbox_logger_p.h"

using namespace gearbox;
using namespace std::chrono_literals;

namespace
{
    using namespace gearbox::http;

    struct CUrlInitializer
    {
        CUrlInitializer()
        {
            curl_global_init(CURL_GLOBAL_ALL);
        }
        ~CUrlInitializer()
        {
            curl_global_cleanup();
        }
    };
    CUrlInitializer cUrlInitializer;

    Error fromCUrlError(std::int32_t errorCode)
    {
        Error err;
        switch (errorCode)
        {
        default:
            err.errorCode = Error::Code::UnknownError;
            err.message = "An unknown error has occured";
            break;
        case CURLE_TOO_MANY_REDIRECTS:
        case CURLE_OK:
            err.errorCode = Error::Code::NoError;
            err.message = "";
            break;
        case CURLE_UNSUPPORTED_PROTOCOL:
            err.errorCode = Error::Code::UnsupportedProtocol;
            break;
        case CURLE_URL_MALFORMAT:
            err.errorCode = Error::Code::InvalidURL;
            break;
        case CURLE_COULDNT_RESOLVE_PROXY:
            err.errorCode = Error::Code::ProxyResolutionFailure;
            break;
        case CURLE_COULDNT_RESOLVE_HOST:
            err.errorCode = Error::Code::HostResolutionFailure;
            break;
        case CURLE_COULDNT_CONNECT:
            err.errorCode = Error::Code::ConnectionFailure;
            break;
        case CURLE_OPERATION_TIMEDOUT:
            err.errorCode = Error::Code::OperationTimedOut;
            break;
        case CURLE_SSL_CONNECT_ERROR:
            err.errorCode = Error::Code::SSLConnectError;
            break;
        case CURLE_PEER_FAILED_VERIFICATION:
            err.errorCode = Error::Code::SSLRemoteCertificateError;
            break;
        case CURLE_GOT_NOTHING:
            err.errorCode = Error::Code::ResponseInvalid;
            break;
        case CURLE_SSL_ENGINE_NOTFOUND:
            err.errorCode = Error::Code::GenericSSLError;
            break;
        case CURLE_SSL_ENGINE_SETFAILED:
            err.errorCode = Error::Code::GenericSSLError;
            break;
        case CURLE_SEND_ERROR:
            err.errorCode = Error::Code::NetworkSendFailure;
            break;
        case CURLE_RECV_ERROR:
            err.errorCode = Error::Code::NetworkReceiveError;
            break;
        case CURLE_SSL_CERTPROBLEM:
            err.errorCode = Error::Code::SSLLocalCertificateError;
            break;
        case CURLE_SSL_CIPHER:
            err.errorCode = Error::Code::GenericSSLError;
            break;
        case CURLE_SSL_CACERT:
            err.errorCode = Error::Code::SSLCacertError;
            break;
        case CURLE_USE_SSL_FAILED:
            err.errorCode = Error::Code::GenericSSLError;
            break;
        case CURLE_SSL_ENGINE_INITFAILED:
            err.errorCode = Error::Code::GenericSSLError;
            break;
        case CURLE_SSL_CACERT_BADFILE:
            err.errorCode = Error::Code::SSLCacertError;
            break;
        case CURLE_SSL_SHUTDOWN_FAILED:
            err.errorCode = Error::Code::GenericSSLError;
            break;
        case CURLE_SSL_CRL_BADFILE:
            err.errorCode = Error::Code::SSLCacertError;
            break;
        case CURLE_SSL_ISSUER_ERROR:
            err.errorCode = Error::Code::SSLCacertError;
            break;
        }

        if ((err.errorCode != Error::Code::NoError) && (err.errorCode != Error::Code::UnknownError))
            err.message = curl_easy_strerror(static_cast<CURLcode>(errorCode));

        return err;
    }

    /* Appends a buffer of length size * nmemb to data. This function is called by CUrl for */
    /* subsequent chunks of data that constitute the response for a request.                */
    std::size_t writeCallback(void *ptr, std::size_t size, std::size_t nmemb, std::string *data)
    {
        data->append(static_cast<char *>(ptr), size * nmemb);
        return size * nmemb;
    }

    /* Appends a key-value pair, represented by ptr, of length size * nmemb, to the data map. */
    /* This function is called by CUrl for each line in the header of a response.             */
    std::size_t headerCallback(void *ptr, std::size_t size, std::size_t nmemb, header_array_t *data)
    {
        std::string header(static_cast<char *>(ptr), size * nmemb);
        auto found = header.find("HTTP/");
        if (found == std::string::npos)
        {
            auto separatorPos = header.find_first_of(':');
            if (separatorPos != std::string::npos)
            {
                auto value = header.substr(separatorPos + 2);
                (*data)[header.substr(0, separatorPos)] = value.substr(0, value.size() - 2);
                /* Ignore CL/RF at the end of the header line             ^^^^^^^^^^^^^^^^ */
            }
        }
        else
        {
            data->clear();
        }
        return size * nmemb;
    }
}

CUrlHttp::CUrlHttp(const std::string &userAgent) :
    hostname_(),
    port_(-1),
    path_("/"),
    authenticationEnabled_(false),
    authentication_(),
    sslErrorHandlingEnabled_(true),
    timeout_(),
    handle_(nullptr)
{
    handle_ = curl_easy_init();
    if (handle_ != nullptr)
    {
        curl_easy_setopt(handle_, CURLOPT_USERAGENT,      userAgent.c_str());
        curl_easy_setopt(handle_, CURLOPT_NOPROGRESS,     1L);
        curl_easy_setopt(handle_, CURLOPT_MAXREDIRS,      50L);
        curl_easy_setopt(handle_, CURLOPT_NOSIGNAL,       1L);
        curl_easy_setopt(handle_, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(handle_, CURLOPT_TCP_KEEPALIVE,  1L);
        curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION,  &writeCallback);
        curl_easy_setopt(handle_, CURLOPT_HEADERFUNCTION, &headerCallback);
    }
    else
    {
        LOG_FATAL("Error {} while initiation connection.", static_cast<std::string>(fromCUrlError(-1)));
    }
}

gearbox::CUrlHttp::~CUrlHttp()
{
    if (handle_ != nullptr)
    {
        curl_easy_cleanup(handle_);
    }
}

const std::string &CUrlHttp::host() const
{
    return hostname_;
}

void CUrlHttp::setHost(const std::string &hostname)
{
    hostname_ = hostname;
}

void CUrlHttp::setHost(std::string &&hostname)
{
    hostname_ = std::move(hostname);
}

CUrlHttp::http_port_t CUrlHttp::port() const
{
    return port_;
}

void CUrlHttp::setPort(http_port_t port)
{
    port_ = port;
}

const std::string &CUrlHttp::path() const
{
    return path_;
}

void CUrlHttp::setPath(const std::string &path)
{
    path_ = path;
}

void CUrlHttp::setPath(std::string &&path)
{
    path_ = std::move(path);
}

bool CUrlHttp::authenticationRequired() const
{
    return authenticationEnabled_;
}

void CUrlHttp::enableAuthentication()
{
    authenticationEnabled_ = true;
}

void CUrlHttp::disableAuthentication()
{
    authenticationEnabled_ = false;
}

const std::string &CUrlHttp::username() const
{
    return authentication_.username;
}

void CUrlHttp::setUsername(const std::string &username)
{
    authenticationEnabled_ = true;
    authentication_.username = username;
}

void CUrlHttp::setUsername(std::string &&username)
{
    authenticationEnabled_ = true;
    authentication_.username = std::move(username);
}

const std::string &CUrlHttp::password() const
{
    return authentication_.password;
}

void CUrlHttp::setPassword(const std::string &password)
{
    authenticationEnabled_ = true;
    authentication_.password = password;
}

void CUrlHttp::setPassword(std::string &&password)
{
    authenticationEnabled_ = true;
    authentication_.password = std::move(password);
}

void CUrlHttp::setSSLErrorHandling(http_ssl_error_handling_t value)
{
    sslErrorHandlingEnabled_ = (value == http_ssl_error_handling_t::Aknowledge);
    if (!sslErrorHandlingEnabled_)
    {
        curl_easy_setopt(handle_, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(handle_, CURLOPT_SSL_VERIFYHOST, 0L);
    }
    else
    {
        curl_easy_setopt(handle_, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(handle_, CURLOPT_SSL_VERIFYHOST, 1L);
    }

}

const milliseconds_t &CUrlHttp::timeout() const
{
    return timeout_;
}

void CUrlHttp::setTimeout(milliseconds_t value)
{
    timeout_ = value;
    curl_easy_setopt(handle_, CURLOPT_TIMEOUT_MS, timeout_.count());
}

CUrlHttp::Request::Request(CURL *handle) :
    handle_(handle),
    headers_()
{
}

CUrlHttp::Request::~Request()
{
    if (handle_ != nullptr)
    {
        curl_easy_cleanup(handle_);
    }
}

void gearbox::CUrlHttp::Request::setBody(const std::string &data)
{
    curl_easy_setopt(handle_, CURLOPT_POSTFIELDSIZE, data.size());
    curl_easy_setopt(handle_, CURLOPT_COPYPOSTFIELDS, data.c_str());
}

void gearbox::CUrlHttp::Request::setHeaders(const CUrlHttp::http_header_array_t &headers)
{
    for (const auto &header: headers)
    {
        headers_[header.first] = header.second;
    }
}

void gearbox::CUrlHttp::Request::setHeader(const CUrlHttp::http_header_t &header)
{
    headers_[header.first] = header.second;
}

CUrlHttp::http_request_result_t CUrlHttp::Request::send()
{
    using namespace gearbox::http;
    std::int32_t httpStatus = gearbox::http::Status::Unknown;
    http_header_array_t responseHeaders;
    std::string text;
    http_error_t err = { Error::Code::InternalError, "An internal connection handle is invalid. "
                  "This is most likely due to operating an a moved or otherwise invalidated "
                  "instance of this object" };
    double elapsed = 0;

    if (handle_ != nullptr)
    {
        curl_slist *headers = nullptr;;
        std::vector<std::string> formatedHeaders;

        for (const auto &header: headers_)
        {
            formatedHeaders.emplace_back(fmt::format("{}: {}", header.first, header.second));
            headers = curl_slist_append(headers, formatedHeaders.back().c_str());
        }
        curl_easy_setopt(handle_, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &text);
        curl_easy_setopt(handle_, CURLOPT_HEADERDATA, &responseHeaders);

        auto errCode = curl_easy_perform(handle_);
        err = fromCUrlError(errCode);

        curl_easy_getinfo(handle_, CURLINFO_RESPONSE_CODE, &httpStatus);
        curl_easy_getinfo(handle_, CURLINFO_TOTAL_TIME, &elapsed);

        if (text.back() == '\n')
        {
            text = text.substr(0, text.size() - 1);
        }

        curl_slist_free_all(headers);
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

CUrlHttp::Request CUrlHttp::createRequest()
{
    auto curl = curl_easy_duphandle(handle_);

    std::string url = fmt::format("{}{}{}", hostname_, port_ > 0 ? fmt::format(":{}", port_) : "", path_);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    if (authenticationEnabled_)
    {
        curl_easy_setopt(curl,
                         CURLOPT_USERPWD,
                         fmt::format("{}:{}", authentication_.username, authentication_.password).c_str());
    }

    return { curl };
}

#endif // PLATFORM_LINUX
