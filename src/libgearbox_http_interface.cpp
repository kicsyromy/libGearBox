#include "libgearbox_http_interface_p.h"

#include <algorithm>
#include <cctype>

using namespace librt::http;

librt::http::Status::Status(std::int32_t code) :
    statusCode_(Unknown),
    name_(nullptr)
{
    switch (code)
    {
    default:
        name_ = "Unknown";
        break;
    case 200:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "OK";
        break;
    case 201:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Created";
        break;
    case 202:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Accepted";
        break;
    case 203:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Non-Authoritative Information";
        break;
    case 204: 
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "No Content";
        break;
    case 205:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Reset Content";
        break;
    case 206:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Partial Content";
        break;
    case 226:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "IM Used";
        break;
    case 300:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Multiple Choices";
        break;
    case 301:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Moved Permanently";
        break;
    case 302:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Found";
        break;
    case 303:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "See Other";
        break;
    case 304:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Not Modified";
        break;
    case 305:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Use Proxy";
        break;
    case 306:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Switch Proxy";
        break;
    case 307:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Temporary Redirect";
        break;
    case 308:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Permanent Redirect";
        break;
    case 400:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Bad Request";
        break;
    case 401:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Unauthorized";
        break;
    case 402:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Payment Required";
        break;
    case 403:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Forbidden";
        break;
    case 404:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Not Found";
        break;
    case 405:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Method Not Allowed";
        break;
    case 406:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Not Acceptable";
        break;
    case 407:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Proxy Authentication Required";
        break;
    case 408:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Request Time-Out";
        break;
    case 409:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Conflict";
        break;
    case 410:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Gone";
        break;
    case 411:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Length Required";
        break;
    case 412:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Precondition Failed";
        break;
    case 413:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Payload Too Large";
        break;
    case 414:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "URI Too Long";
        break;
    case 415:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Unsoported Media Type";
        break;
    case 416:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Range Not Satisfiable";
        break;
    case 417:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Expectation Failed";
        break;
    case 421:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Misdirected Request";
        break;
    case 426:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Upgrade Required";
        break;
    case 428:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Precondition Required";
        break;
    case 429:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Too Many Requests";
        break;
    case 431:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Request Header Fields Too Large";
        break;
    case 451:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Unavailable For Legal Reasons";
        break;
    case 500:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Internal Server Error";
        break;
    case 501:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Not Implemented";
        break;
    case 502:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Bad Gateway";
        break;
    case 503:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Service Unavailable";
        break;
    case 504:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Gateway Time-Out";
        break;
    case 505:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "HTTP Version Not Supported";
        break;
    case 506:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Variant Also Negotiates";
        break;
    case 510:
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Not Extended";
        break;
    case 511: 
        statusCode_ = static_cast<Status::Code>(code);
        name_ = "Network Authentication Required";
        break;
    }
}
