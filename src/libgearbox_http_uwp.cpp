#include "include\libgearbox_http_uwp_p.h"
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

#ifdef PLATFORM_UWP

#include "libgearbox_http_uwp_p.h"

using namespace gearbox;

HttpUWP::HttpUWP(const std::string &userAgent)
{
}

HttpUWP::HttpUWP(HttpUWP &&other) noexcept(true)
{
}

gearbox::HttpUWP::~HttpUWP()
{
}

const std::string &HttpUWP::host() const
{
    return {};
}

void HttpUWP::setHost(const std::string &hostname)
{
}

void HttpUWP::setHost(std::string &&hostname)
{
}

HttpUWP::http_port_t HttpUWP::port() const
{
    return {};
}

void HttpUWP::setPort(http_port_t port)
{
}

const std::string &HttpUWP::path() const
{
    return {};
}

void HttpUWP::setPath(const std::string &path)
{
}

void HttpUWP::setPath(std::string &&path)
{
}

bool HttpUWP::authenticationRequired() const
{
    return false;
}

void HttpUWP::enableAuthentication()
{
}

void HttpUWP::disableAuthentication()
{
}

const std::string & HttpUWP::username() const
{
    return {};
}

void HttpUWP::setUsername(const std::string &username)
{
}

void HttpUWP::setUsername(std::string &&username)
{
}

const std::string &HttpUWP::password() const
{
    return {};
}

void HttpUWP::setPassword(const std::string &password)
{
}

void HttpUWP::setPassword(std::string &&password)
{
}

void HttpUWP::setSSLErrorHandling(http_ssl_error_handling_t value)
{
}

const HttpUWP::milliseconds_t &HttpUWP::timeout() const
{
    return {};
}

void gearbox::HttpUWP::setTimeout(milliseconds_t value)
{
}

HttpUWP::Request::Request()
{
}

HttpUWP::Request::~Request()
{
}

void HttpUWP::Request::setBody(const std::string &data)
{
}

void HttpUWP::Request::setHeaders(const http_header_array_t &headers)
{
}

void HttpUWP::Request::setHeader(const http_header_t &header)
{
}

HttpUWP::http_request_result_t HttpUWP::Request::send()
{
    return { http_status_t { 999 } };
}

HttpUWP::Request HttpUWP::createRequest()
{
    return {};
}

#endif // PLATFORM_UWP
