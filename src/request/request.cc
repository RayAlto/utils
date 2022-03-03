#include "request/request.h"

#include <cstring> // std::strlen, std::size_t
#include <ctime>
#include <iostream>
#include <string>
#include <utility>

#include <curl/curl.h>
#include <curl/easy.h>

#include "request/header.h"
#include "request/method.h"
#include "request/response.h"
#include "string/strtool.h"

namespace rayalto {
namespace utils {

namespace {

void parse_header(const char* data,
                  const std::size_t& size,
                  request::Header& result) {
    for (std::string& header : string::split(std::string(data, size), '\n')) {
        if (header.empty()) {
            // empty line
            continue;
        }
        if (header.substr(0, 5) == "HTTP/") {
            // something like 'HTTP/2 200'
            continue;
        }
        std::size_t colon = header.find(':');
        if (colon == std::string::npos) {
            // failed to locate ':'
            continue;
        }
        result[string::strip(header.substr(0, colon))] =
            string::strip(header.substr(colon + 1));
    }
}

std::size_t curl_custom_write_function(char* ptr,
                                       std::size_t size,
                                       std::size_t nmemb,
                                       std::string* userdata) {
    std::size_t actual_size = size * nmemb;
    userdata->append(ptr, actual_size);
    return actual_size;
}

std::size_t curl_custom_header_function(char* buffer,
                                        std::size_t size,
                                        std::size_t nitems,
                                        request::Header* userdata) {
    std::size_t actual_size = size * nitems;
    parse_header(buffer, actual_size, *userdata);
    return actual_size;
}

} // anonymous namespace

Request::Request() {
    curl_global_init(CURL_GLOBAL_ALL);
    handle_ = curl_easy_init();
    curl_version_ = curl_version_info(CURLVERSION_NOW)->version;
    url("https://httpbin.org/anything");
    // url("https://httpbin.org/basic-auth/foo/bar");
    // url("https://httpbin.org/hidden-basic-auth/foo/bar");
    // url("https://httpbin.org/cookies");
    // url("https://httpbin.org/cookies/set/test/test");
    // url("https://httpbin.org/headers");
    // url("https://www.baidu.com");
    cookie({{"a", "1"}, {"b", "2"}});
    useragent("114514");
    header({{"Test", "test"}, {"Foo", "bar"}});
    authentication({"foo", "bar"});
    method(request::Method::POST);
    request();
    auto r = response_;
    std::cout << "===== Code =====" << std::endl;
    std::cout << r.code << std::endl;
    std::cout << "===== Body =====" << std::endl;
    std::cout << r.body << std::endl;
    std::cout << "===== Header =====" << std::endl;
    for (auto p : r.header) {
        std::cout << p.first << ": " << p.second << std::endl;
    }
    std::cout << "===== Cookie =====" << std::endl;
    for (auto p : r.cookie) {
        std::cout << p.first << ": " << p.second << std::endl;
    }
    std::cout << "===== Byte Transferd =====" << std::endl;
    std::cout << "size downloaded: " << r.byte_transfered.download << std::endl;
    std::cout << "download speed: " << r.speed.download << std::endl;
    std::cout << "===== Local Info =====" << std::endl;
    std::cout << "ip: " << r.local_info.ip << std::endl;
    std::cout << "port: " << r.local_info.port << std::endl;
}

Request::~Request() {
    curl_easy_cleanup(handle_);
    curl_global_cleanup();
}

const std::string Request::curl_version() {
    return curl_version_;
}

void Request::reset() {
    curl_easy_reset(handle_);
    authentication_.clear();
    header_.clear();
    cookie_.clear();
}

request::Method Request::method() {
    return method_;
}

void Request::method(const request::Method& method) {
    method_ = method;
}

std::string Request::url() {
    return url_;
}

void Request::url(const std::string& url) {
    url_ = url;
}

void Request::url(std::string&& url) {
    url_ = std::move(url);
}

request::Cookie Request::cookie() {
    return cookie_;
}

void Request::cookie(const request::Cookie& cookie) {
    cookie_ = cookie;
}

void Request::cookie(request::Cookie&& cookie) {
    cookie_ = std::move(cookie);
}

request::Header Request::header() {
    return header_;
}

void Request::header(const request::Header& header) {
    header_ = header;
}

void Request::header(request::Header&& header) {
    header_ = std::move(header);
}

std::string Request::useragent() {
    return useragent_;
}

void Request::useragent(const std::string& useragent) {
    useragent_ = useragent;
}

void Request::useragent(std::string&& useragent) {
    useragent_ = std::move(useragent);
}

request::Authentication Request::authentication() {
    return authentication_;
}

void Request::authentication(const request::Authentication& authentication) {
    authentication_ = authentication;
}

void Request::authentication(request::Authentication&& authentication) {
    authentication_ = std::move(authentication);
}

bool Request::request() {
    char error_info_buffer[CURL_ERROR_SIZE];
    error_info_buffer[0] = 0;
    // [option] verbose
    if (verbose_) {
        curl_easy_setopt(handle_, CURLOPT_VERBOSE, 1);
    }
    // [option] url
    curl_easy_setopt(handle_, CURLOPT_URL, url_.c_str());
    // [option] cookie
    curl_easy_setopt(handle_, CURLOPT_COOKIE, cookie_.c_str());
    // [option] header
    curl_easy_setopt(handle_, CURLOPT_HTTPHEADER, header_.curl_header());
    // [option] useragent
    curl_easy_setopt(handle_, CURLOPT_USERAGENT, useragent_.c_str());
    // [option] authentication
    curl_easy_setopt(handle_, CURLOPT_USERPWD, authentication_.c_str());
    // [option] method
    curl_easy_setopt(
        handle_, CURLOPT_CUSTOMREQUEST, request::method::c_str(method_));
    // receive cookie
    curl_easy_setopt(handle_, CURLOPT_COOKIEFILE, "");
    // receive cert info
    curl_easy_setopt(handle_, CURLOPT_CERTINFO, 1l);
    // receive error
    curl_easy_setopt(handle_, CURLOPT_ERRORBUFFER, error_info_buffer);
    // receive response text
    curl_easy_setopt(
        handle_, CURLOPT_WRITEFUNCTION, curl_custom_write_function);
    curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &response_.body);
    // receive response header
    curl_easy_setopt(
        handle_, CURLOPT_HEADERFUNCTION, curl_custom_header_function);
    curl_easy_setopt(handle_, CURLOPT_HEADERDATA, &response_.header);
    // perform
    CURLcode curl_result = curl_easy_perform(handle_);
    // receive message
    response_.message = (std::strlen(error_info_buffer) == 0)
                            ? error_info_buffer
                            : curl_easy_strerror(curl_result);
    // receive response code
    curl_easy_getinfo(handle_, CURLINFO_RESPONSE_CODE, &response_.code);
    // receive response cookie
    curl_slist* curl_cookies = nullptr;
    curl_easy_getinfo(handle_, CURLINFO_COOKIELIST, &curl_cookies);
    response_.cookie.from_curl_slist(curl_cookies);
    curl_slist_free_all(curl_cookies);
    // receive connection http version
    curl_easy_getinfo(handle_, CURLINFO_HTTP_VERSION, &response_.http_version);
    // receive time elapsed
    curl_easy_getinfo(
        handle_, CURLINFO_TOTAL_TIME, &response_.time_elapsed.all);
    curl_easy_getinfo(handle_,
                      CURLINFO_NAMELOOKUP_TIME,
                      &response_.time_elapsed.name_resolve);
    curl_easy_getinfo(
        handle_, CURLINFO_CONNECT_TIME, &response_.time_elapsed.connect);
    curl_easy_getinfo(
        handle_, CURLINFO_APPCONNECT_TIME, &response_.time_elapsed.handshake);
    curl_easy_getinfo(handle_,
                      CURLINFO_PRETRANSFER_TIME,
                      &response_.time_elapsed.pre_transfer);
    curl_easy_getinfo(handle_,
                      CURLINFO_STARTTRANSFER_TIME,
                      &response_.time_elapsed.start_transfer);
    curl_easy_getinfo(
        handle_, CURLINFO_SIZE_UPLOAD_T, &response_.byte_transfered.upload);
    curl_easy_getinfo(
        handle_, CURLINFO_SIZE_DOWNLOAD_T, &response_.byte_transfered.download);
    curl_easy_getinfo(
        handle_, CURLINFO_SPEED_UPLOAD_T, &response_.speed.upload);
    curl_easy_getinfo(
        handle_, CURLINFO_SPEED_DOWNLOAD_T, &response_.speed.download);
    char* local_ip;
    curl_easy_getinfo(handle_, CURLINFO_LOCAL_IP, &local_ip);
    response_.local_info.ip = local_ip;
    curl_easy_getinfo(handle_, CURLINFO_LOCAL_PORT, &response_.local_info.port);
    return (curl_result == CURLE_OK);
}

request::Response Request::response() {
    return response_;
}

std::time_t Request::parse_time_str(const char* time_str) {
    return curl_getdate(time_str, nullptr);
}

} // namespace utils
} // namespace rayalto
