#ifndef RA_UTILS_REQUEST_REQUEST_HPP_
#define RA_UTILS_REQUEST_REQUEST_HPP_

#include <cstddef>
#include <cstdio>
#include <ctime>
#include <string>

#include <curl/curl.h>

#include "request/authentication.h"
#include "request/ip_resolve.h"
#include "request/method.h"
#include "request/proxy.h"
#include "request/mime_parts.h"
#include "request/mime_part.h"
#include "request/response.h"

namespace rayalto {
namespace utils {

struct LocalSetting {
    std::string interface;
    long port = 0;
    long port_range = 1;
    std::string dns_interface;
    std::string dns_local_ipv4;
    std::string dns_local_ipv6;
};

struct TimeoutSetting {
    long timeout = 0l;
    long connect_timeout = 300000l;
};

/**
 * Make http request easily, super heavy shitty wrapper for the great curl
 *
 * Note:
 *   - Set mime type in body and mime_parts at the same time may cause strange
 *       problems.
 */
class Request {
public:
    Request();
    Request(const Request&) = delete;
    Request(Request&&) noexcept = delete;
    Request& operator=(const Request&) = delete;
    Request& operator=(Request&&) noexcept = delete;

    virtual ~Request();

    // get curl version string
    static std::string curl_version();
    // reset all current setting
    void reset();
    // get current http method
    request::Method& method();
    const request::Method& method() const;
    // set http method
    void method(const request::Method& method);

    // current ip resolve setting
    request::IP_Resolve& ip_resolve();
    const request::IP_Resolve& ip_resolve() const;
    // want IPv6? ip_resolve(request::IP_RESOLVE::IPv6_ONLY)
    void ip_resolve(const request::IP_Resolve& ip_resolve);

    // get current target url
    std::string& url();
    const std::string& url() const;
    // set current target url
    void url(const std::string& url);
    void url(std::string&& url);

    // get current cookie for request
    request::Cookie& cookie();
    const request::Cookie& cookie() const;
    // set current cookie for request
    void cookie(const request::Cookie& cookie);
    void cookie(request::Cookie&& cookie);

    // get current header for request
    request::Header& header();
    const request::Header& header() const;
    // set current header for request
    void header(const request::Header& header);
    void header(request::Header&& header);

    // get current user agent setting
    std::string& useragent();
    const std::string& useragent() const;
    // set user agent (set user agent header will override this)
    void useragent(const std::string& useragent);
    void useragent(std::string&& useragent);

    // get current server authentication setting
    request::Authentication& authentication();
    const request::Authentication& authentication() const;
    // set server authentication (set authorization header will override this)
    void authentication(const request::Authentication& authentication);
    void authentication(request::Authentication&& authentication);

    // get body set for the current request
    std::string& body();
    const std::string& body() const;
    // set body for current request, default mime type is
    // "application/x-www-form-urlencoded"
    void body(
        const std::string& body,
        const std::string& mime_type = "application/x-www-form-urlencoded");
    void body(std::string&& body,
              std::string&& mime_type = "application/x-www-form-urlencoded");

    // multipart/formdata
    request::MimeParts& mime_parts();
    const request::MimeParts& mime_parts() const;
    void mime_parts(const request::MimeParts& mime_parts);
    void mime_parts(request::MimeParts&& mime_parts);

    // get current proxy setting
    request::Proxy& proxy();
    const request::Proxy& proxy() const;
    // set proxy server
    void proxy(const request::Proxy& proxy);
    void proxy(request::Proxy&& proxy);

    // get timeout (milliseconds) setting for current transfer operation
    //     default: 0, never times out during transfer
    long& timeout();
    const long& timeout() const;
    // timeout (milliseconds) for transfer operation
    //     default: 0, never times out during transfer
    void timeout(const long& timeout);

    // get timeout (milliseconds) setting for connection phase
    //     default: 300000
    long& connect_timeout();
    const long& connect_timeout() const;
    // set timeout (milliseconds) for connection phase
    //     default: 300000
    void connect_timeout(const long& connect_timeout);

    // get current outgoing network interface setting
    std::string& local_interface();
    const std::string& local_interface() const;
    // set outgoing network interface (interface name/IP address/host name)
    void local_interface(const std::string& interface);
    void local_interface(std::string&& interface);

    // local port setting
    long& local_port();
    const long& local_port() const;
    // local port used for the connection
    //     default: 0, use whatever the system thinks is fine
    void local_port(const long& port);
    void local_port(long&& port);

    // local port range setting
    long& local_port_range();
    const long& local_port_range() const;
    // local port range to try
    //     default: 1
    void local_port_range(const long& range);
    void local_port_range(long&& range);

    // get current setting for interface DNS resolver should bind to
    std::string& dns_interface();
    const std::string& dns_interface() const;
    // set interface DNS resolver should bind to
    // (MUST be an interface name, ip address/host name is NOT allowed)
    void dns_interface(const std::string& dns_interface);
    void dns_interface(std::string&& dns_interface);

    // get current setting for local ipv4 address DNS resolver should bind to
    std::string& dns_local_ipv4();
    const std::string& dns_local_ipv4() const;
    // set local ipv4 address DNS resolver should bind to
    void dns_local_ipv4(const std::string& dns_local_ipv4);
    void dns_local_ipv4(std::string&& dns_local_ipv4);

    // get current setting for local ipv6 address DNS resolver should bind to
    std::string& dns_local_ipv6();
    const std::string& dns_local_ipv6() const;
    // set local ipv6 address DNS resolver should bind to
    void dns_local_ipv6(const std::string& dns_local_ipv6);
    void dns_local_ipv6(std::string&& dns_local_ipv6);

    // apply current settings and perform the request
    bool request();

    // get the last response
    request::Response& response();
    const request::Response& response() const;

    /* ===== other func ===== */
    std::string url_encode(const std::string& url);
    std::string url_encode(const char* url, std::size_t len = 0);
    std::string url_decode(const std::string& url);
    std::string url_decode(const char* url, std::size_t len = 0);

protected:
    CURL* handle_;

    char error_info_buffer[CURL_ERROR_SIZE];
    std::FILE* temp_stderr = nullptr;

    request::Method method_ = request::Method::DEFAULT;
    request::IP_Resolve ip_resolve_ = request::IP_Resolve::WHATEVER;
    std::string url_;
    request::Cookie cookie_;
    request::Header header_;
    std::string useragent_;
    request::Authentication authentication_;
    std::string body_;
    request::MimeParts mime_parts_;
    // why the FUCK curl_mime_init() need a curl handle?
    curl_mime* curl_mime_ = nullptr;
    request::Proxy proxy_;
    LocalSetting local_setting_;
    TimeoutSetting timeout_setting_;

    request::Response response_;

    const static std::string curl_version_;

    void init_curl_handle_();
    void set_options_();
    bool perform_request_();
};

/* ========== other func ========== */

namespace request {

/**
 * parse a time string to unix timestamp
 * see: https://curl.se/libcurl/c/curl_getdate.html
 */
std::time_t parse_time_str(const char* time_str);

} // namespace request

} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_REQUEST_HPP_
