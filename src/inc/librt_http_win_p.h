#ifndef LIBRT_HTTP_WIN_P_H
#define LIBRT_HTTP_WIN_P_H

#include <windows.h>
#include <wininet.h>

#include "librt_global.h"

#include "librt_http_interface_p.h"

namespace librt
{
    class WinHttp
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
        explicit WinHttp(const std::string &userAgent);
        WinHttp(WinHttp &&other) noexcept(true);
        WinHttp &operator =(WinHttp &&) noexcept(true) = default;
        ~WinHttp();

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
            Request(HINTERNET session,
                    const std::string &path,
                    DWORD connectionFlags,
                    DWORD_PTR &requestId);
            Request(const Request &);             /* Visual Studio thinks it needs this */
            Request &operator =(const Request &); /* Visual Studio thinks it needs this */
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
            HINTERNET session_;
            const std::string &path_;
            HttpRequestType requestType_;
            DWORD connectionFlags_;
            DWORD_PTR &requestId_;
            HttpHeaderArray headers_;
            char *data_;
            DWORD dataSize_;
        };
        Request createRequest();

    private:
        void closeSession();

    private:
        std::string hostname_;
        HttpPort port_;
        std::string path_;
        bool authenticationEnabled_;
        struct { std::string username; std::string password; } authentication_;
        bool sslErrorHandlingEnabled_;
        Milliseconds timeout_;

    private:
        HINTERNET connection_;
        HINTERNET session_;
        DWORD_PTR requestId_;

    private:
        int connectionFlags_;

    private:
        DISABLE_COPY(WinHttp)
    };
}

#endif // LIBRT_HTTP_WIN_P_H 
