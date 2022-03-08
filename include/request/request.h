#ifndef RA_UTILS_REQUEST_REQUEST_HPP_
#define RA_UTILS_REQUEST_REQUEST_HPP_

#include <cstddef>
#include <ctime>
#include <string>

#include <curl/curl.h>

#include "authentication.h"
#include "ip_resolve.h"
#include "method.h"
#include "proxy.h"
#include "response.h"

namespace rayalto {
namespace utils {

/**
 * Make http request easily
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
    request::Method method();
    // set http method
    void method(const request::Method& method);
    // ip resolve
    request::IP_Resolve ip_resolve();
    // want to try IPv6? ip_resolve(request::IP_RESOLVE::IPv6_ONLY)
    void ip_resolve(const request::IP_Resolve& ip_resolve);
    // get current target url
    std::string url();
    // set target url
    void url(const std::string& url);
    // set target url
    void url(std::string&& url);
    // get current cookie
    request::Cookie cookie();
    // set cookie
    void cookie(const request::Cookie& cookie);
    // set cookie
    void cookie(request::Cookie&& cookie);
    // get current header
    request::Header header();
    // set header
    void header(const request::Header& header);
    // set header
    void header(request::Header&& header);
    // get current user agent
    std::string useragent();
    // set user agent (set user agent header will override this)
    void useragent(const std::string& useragent);
    // set user agent (set user agent header will override this)
    void useragent(std::string&& useragent);
    // get current server authentication setting
    request::Authentication authentication();
    // set server authentication (set authorization header will override this)
    void authentication(const request::Authentication& authentication);
    // set server authentication (set authorization header will override this)
    void authentication(request::Authentication&& authentication);
    // get current proxy server
    request::Proxy proxy();
    // set proxy server
    void proxy(const request::Proxy& proxy);
    // set proxy server
    void proxy(request::Proxy&& proxy);
    // tunneling through http proxy set with Request::proxy()
    bool http_proxy_tunnel();
    // tunneling through http proxy set with Request::proxy()
    void http_proxy_tunnel(const bool& tunneling);
    // timeout for transfer operation
    long timeout();
    // timeout for transfer operation
    void timeout(const long& timeout);
    // timeout for connection phase
    long connect_timeout();
    // timeout for connection phase
    void connect_timeout(const long& connect_timeout);

    // apply current settings and perform the request
    bool request();

    // get the last response
    request::Response response();

    /* ===== other func ===== */
    std::string url_encode(const std::string& url);
    std::string url_encode(const char* url, std::size_t len = 0);
    std::string url_decode(const std::string& url);
    std::string url_decode(const char* url, std::size_t len = 0);

protected:
    CURL* handle_;
    bool verbose_ = false;

    request::Method method_ = request::Method::DEFAULT;
    request::IP_Resolve ip_resolve_ = request::IP_Resolve::WHATEVER;
    std::string url_;
    request::Cookie cookie_;
    request::Header header_;
    std::string useragent_;
    request::Authentication authentication_;
    request::Proxy proxy_;
    bool http_proxy_tunnel_ = false;
    long timeout_ = 0l;
    long connect_timeout_ = 0l;

    request::Response response_;

    const static std::string curl_version_;
};

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
