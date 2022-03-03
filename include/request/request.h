#ifndef RA_UTILS_REQUEST_REQUEST_HPP_
#define RA_UTILS_REQUEST_REQUEST_HPP_

#include <cstddef>
#include <ctime>
#include <string>

#include <curl/curl.h>

#include "authentication.h"
#include "method.h"
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
    const std::string curl_version();
    // reset all current setting
    void reset();
    // get current http method
    request::Method method();
    // set http method
    void method(const request::Method& method);
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

    // apply current settings and perform the request
    bool request();

    // get the last response
    request::Response response();

    /* ===== other func ===== */
    /**
     * parse a time string to unix timestamp
     * see: https://curl.se/libcurl/c/curl_getdate.html
     */
    std::time_t parse_time_str(const char* time_str);

protected:
    CURL* handle_;
    bool verbose_ = false;

    request::Method method_ = request::Method::GET;
    std::string url_;
    request::Cookie cookie_;
    request::Header header_;
    std::string useragent_;
    request::Authentication authentication_;
    request::Response response_;

    std::string curl_version_;
};

} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_REQUEST_HPP_
