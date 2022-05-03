#ifndef RA_UTILS_REQUEST_REQUEST_HPP_
#define RA_UTILS_REQUEST_REQUEST_HPP_

#include <ctime>
#include <memory>
#include <string>

#include "request/authentication.h"
#include "request/cookie.h"
#include "request/header.h"
#include "request/ip_resolve.h"
#include "request/mime_parts.h"
#include "request/proxy.h"
#include "request/response.h"

namespace rayalto {
namespace utils {

/**
 * Make http request easily, super heavy shitty wrapper for the great curl
 *
 * Note:
 *   - Set mime type in body and mime_parts at the same time may cause strange
 *       problems.
 */
class Request {
public:
    enum class Method : std::uint8_t { GET, POST, PUT, DELETE, PATCH, DEFAULT };
    static constexpr const char* method_c_str(const Method& method);

public:
    Request();
    Request(const Request&) = delete;
    Request(Request&&) noexcept = default;
    Request& operator=(const Request&) = delete;
    Request& operator=(Request&&) noexcept = default;

    virtual ~Request();

    // get curl version string
    static std::string curl_version();
    // reset all current setting
    void reset();

    // get current request method
    Method method();
    // set request method
    Request& method(Method method);

    // get current setting for ip resolve (whatever/IPv4 only/IPv6 only)
    request::IpResolve ip_resolve();
    // select the kind of IP address to use (whatever/IPv4 only/IPv6 only)
    Request& ip_resolve(request::IpResolve ip_resolve);

    // get current target url
    std::string url();
    // set target url
    Request& url(const std::string& url);
    Request& url(std::string&& url);

    // get cookie to send for current request
    request::Cookie cookie();
    // set cookie to send
    Request& cookie(const request::Cookie& cookie);
    Request& cookie(request::Cookie&& cookie);

    // get header for current request
    request::Header header();
    // set header for current request
    Request& header(const request::Header& header);
    Request& header(request::Header&& header);

    // get current user agent setting
    std::string useragent();
    // set user agent (set user agent header will override this)
    Request& useragent(const std::string& useragent);
    Request& useragent(std::string&& useragent);

    // get current server authentication setting
    request::Authentication authentication();
    // set server authentication (set authentication header will override this)
    Request& authentication(const request::Authentication& authentication);
    Request& authentication(request::Authentication&& authentication);

    // get body for current request
    std::string body();
    // set body for current request, default mime type is
    // "application/x-www-form-urlencoded"
    Request& body(
        const std::string& body,
        const std::string& mime_type = "application/x-www-form-urlencoded");
    Request& body(
        std::string&& body,
        std::string&& mime_type = "application/x-www-form-urlencoded");

    // get multipart/formdata for current request
    request::MimeParts mime_parts();
    // set multipart/formdata for current request
    Request& mime_parts(const request::MimeParts& mime_parts);
    Request& mime_parts(request::MimeParts&& mime_parts);

    // get proxy setting
    request::Proxy proxy();
    // set proxy
    Request& proxy(const request::Proxy& proxy);
    Request& proxy(request::Proxy&& proxy);

    // get timeout (milliseconds) setting for current transfer operation
    //     default: 0, never times out during transfer
    long timeout();
    // set timeout (milliseconds) setting for current transfer operation
    //     default: 0, never times out during transfer
    Request& timeout(const long& timeout);
    Request& timeout(long&& timeout);

    // get timeout (milliseconds) setting for connection phase
    //     default: 300000
    long connect_timeout();
    // set imeout (milliseconds) setting for connection phase
    //     default: 300000
    Request& connect_timeout(const long& connect_timeout);
    Request& connect_timeout(long&& connect_timeout);

    // get current outgoing network interface setting
    std::string local_interface();
    // set outgoing network interface (interface name/IP address/host name)
    Request& local_interface(const std::string& local_interface);
    Request& local_interface(std::string&& local_interface);

    // get local port setting used for the connection
    long local_port();
    // set local port used for the connection
    //     default: 0, use whatever the system thinks is fine
    Request& local_port(const long& local_port);
    Request& local_port(long&& local_port);

    // get local port range (to try) setting
    long local_port_range();
    // set local port range to try
    //     default: 1, only try one port
    Request& local_port_range(const long& local_port_range);
    Request& local_port_range(long&& local_port_range);

    // get current setting for interface DNS resolver should bind to
    std::string dns_interface();
    // set interface DNS resolver should bind to
    //     (MUST be an interface name, ip address/host name is NOT allowed)
    Request& dns_interface(const std::string& dns_interface);
    Request& dns_interface(std::string&& dns_interface);

    // get current setting for local ipv4 address DNS resolver should bind to
    std::string dns_local_ipv4();
    // set local ipv4 address DNS resolver should bind to
    Request& dns_local_ipv4(const std::string& dns_local_ipv4);
    Request& dns_local_ipv4(std::string&& dns_local_ipv4);

    // get current setting for local ipv6 address DNS resolver should bind to
    std::string dns_local_ipv6();
    // set local ipv6 address DNS resolver should bind to
    Request& dns_local_ipv6(const std::string& dns_local_ipv6);
    Request& dns_local_ipv6(std::string&& dns_local_ipv6);

    // apply all settings in one go, then perform the request
    bool request();

    // get the last response
    request::Response response();

protected:
    class Impl;
    std::unique_ptr<Impl> impl_;
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
