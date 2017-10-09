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

#ifndef LIBGEARBOX_HTTP_UWP_P_H
#define LIBGEARBOX_HTTP_UWP_P_H

#include <memory>

#include <windows.h>

#include "libgearbox_global.h"

#include "libgearbox_http_interface_p.h"

namespace gearbox
{
    class HttpUWP
    {
    private:
        using milliseconds_t = gearbox::http::milliseconds_t;
        using http_header_t = gearbox::http::header_t;
        using http_header_array_t = gearbox::http::header_array_t;
        using http_port_t = gearbox::http::port_t;
        using http_request_t = gearbox::http::RequestType;
        using http_ssl_error_handling_t = gearbox::http::SSLErrorHandling;
        using http_status_t = gearbox::http::Status;
        using http_error_t = gearbox::http::Error;
        using http_request_result_t = gearbox::http::RequestResult;

    public:
        explicit HttpUWP(const std::string &userAgent);
        HttpUWP(HttpUWP &&other) noexcept(true);
        HttpUWP &operator=(HttpUWP &&) noexcept(true) = default;
        ~HttpUWP();

    public:
        const std::string &host() const;
        void setHost(const std::string &hostname);
        void setHost(std::string &&hostname);

        http_port_t port() const;
        void setPort(http_port_t port);

        const std::string &path() const;
        void setPath(const std::string &path);
        void setPath(std::string &&path);

        bool authenticationRequired() const;
        void enableAuthentication();
        void disableAuthentication();
        const std::string &username() const;
        void setUsername(const std::string &username);
        void setUsername(std::string &&username);

        const std::string &password() const;
        void setPassword(const std::string &password);
        void setPassword(std::string &&password);

        void setSSLErrorHandling(http_ssl_error_handling_t value);

        const milliseconds_t &timeout() const;
        void setTimeout(milliseconds_t value);

    public:
        class Request
        {
        public:
            Request();
            Request(const Request &); /* Visual Studio thinks it needs this */
            Request &operator=(
                const Request &); /* Visual Studio thinks it needs this */
            Request(Request &&) noexcept(true) = default;
            Request &operator=(Request &&) noexcept(true) = default;
            ~Request();

        public:
            void setBody(const std::string &data);
            void setHeaders(const http_header_array_t &headers);
            void setHeader(const http_header_t &header);

        public:
            http_request_result_t send();
        };
        Request createRequest();

    private:
        std::string hostname_;
        http_port_t port_;
        std::string path_;
        bool authenticationEnabled_;
        struct
        {
            std::string username;
            std::string password;
        } authentication_;
        bool sslErrorHandlingEnabled_;
        milliseconds_t timeout_;

    private:
        DISABLE_COPY(HttpUWP)
    };
}

#endif // LIBGEARBOX_HTTP_UWP_P_H
