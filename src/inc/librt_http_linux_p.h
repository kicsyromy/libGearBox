#ifndef LIBRT_HTTP_WIN_P_H
#define LIBRT_HTTP_WIN_P_H

#include <atomic>

#include <curl/curl.h>

#include "librt_global.h"
#include "librt_http_interface_p.h"

namespace librt
{
    class CUrlHttp
    {
    private:
        using Milliseconds = librt::http::Milliseconds;
        using HttpHeader = librt::http::Header;
        using HttpHeaderArray = librt::http::HeaderArray;
        using HttpPort = librt::http::Port;
        using HttpRequestType = librt::http::RequestType;
        using HttpSSLErrorHandling = librt::http::SSLErrorHandling;
        using HttpStatus = librt::http::Status;
        using HttpRequestError = librt::http::Error;
        using HttpRequestResult = librt::http::RequestResult;

    public:
        explicit CUrlHttp(const std::string &userAgent);
        CUrlHttp(CUrlHttp&&other) noexcept(true) = default;
        CUrlHttp &operator =(CUrlHttp &&) noexcept(true) = default;
        ~CUrlHttp();

    public:
        const std::string &host() const;
        void setHost(const std::string &hostname);
        void setHost(std::string &&hostname);

        HttpPort port() const;
        void setPort(HttpPort port);

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

        void setSSLErrorHandling(HttpSSLErrorHandling value);

        const Milliseconds &timeout() const;
        void setTimeout(Milliseconds value);

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
            void setHeaders(const HttpHeaderArray &headers);
            void setHeader(const HttpHeader &header);

        public:
            HttpRequestResult send();

        private:
            CURL *handle_;
            HttpHeaderArray headers_;

        private:
            DISABLE_COPY(Request)
        };
        Request createRequest();

    private:
        std::string hostname_;
        HttpPort port_;
        std::string path_;
        bool authenticationEnabled_;
        struct { std::string username; std::string password; } authentication_;
        bool sslErrorHandlingEnabled_;
        Milliseconds timeout_;

    private:
        CURL *handle_;

    private:
        DISABLE_COPY(CUrlHttp)
    };
}

#endif // LIBRT_HTTP_WIN_P_H 
