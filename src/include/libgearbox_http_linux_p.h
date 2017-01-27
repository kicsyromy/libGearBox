#ifndef LIBGEARBOX_HTTP_WIN_P_H
#define LIBGEARBOX_HTTP_WIN_P_H

#include <curl/curl.h>

#include "libgearbox_global.h"
#include "libgearbox_http_interface_p.h"

namespace gearbox
{
    class CUrlHttp
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
        explicit CUrlHttp(const std::string &userAgent);
        CUrlHttp(CUrlHttp &&) noexcept(true) = default;
        CUrlHttp &operator =(CUrlHttp &&) noexcept(true) = default;
        ~CUrlHttp();

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
            Request(CURL *handle);
            Request(Request &&) noexcept(true) = default;
            Request &operator =(Request &&) noexcept(true) = default;
            ~Request();

        public:
            void setBody(const std::string &data);
            void setHeaders(const http_header_array_t &headers);
            void setHeader(const http_header_t &header);

        public:
            http_request_result_t send();

        private:
            CURL *handle_;
            http_header_array_t headers_;

        private:
            DISABLE_COPY(Request)
        };
        Request createRequest();

    private:
        std::string hostname_;
        http_port_t port_;
        std::string path_;
        bool authenticationEnabled_;
        struct { std::string username; std::string password; } authentication_;
        bool sslErrorHandlingEnabled_;
        milliseconds_t timeout_;

    private:
        CURL *handle_;

    private:
        DISABLE_COPY(CUrlHttp)
    };
}

#endif // LIBGEARBOX_HTTP_WIN_P_H 
