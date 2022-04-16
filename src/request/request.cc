#include "request/request.h"

#include <cstddef>
#include <cstdio>
#include <cstring> // std::strlen, std::size_t
#include <ctime>
#include <iostream>
#include <string>
#include <utility>

#include <curl/curl.h>
#include <curl/easy.h>

#include "request/header.h"
#include "request/ip_resolve.h"
#include "request/method.h"
#include "request/mime_part.h"
#include "request/response.h"
#include "string/strtool.h"
#include "util/mime_types.h"

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
        if (header.rfind("HTTP/", 0) == 0) {
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

void curl_add_mime_part(curl_mime* mime,
                        const std::string& part_name,
                        const request::MimePart& mime_part) {
    curl_mimepart* curl_mimepart_ = curl_mime_addpart(mime);
    if (!part_name.empty()) {
        curl_mime_name(curl_mimepart_, part_name.c_str());
    }
    if (!mime_part.type().empty()) {
        curl_mime_type(curl_mimepart_, mime_part.type().c_str());
    }
    if (mime_part.is_file()) {
        // is a file
        if (!mime_part.data().empty()) {
            curl_mime_filedata(curl_mimepart_, mime_part.data().c_str());
        }
        if (!mime_part.file_name().empty()) {
            curl_mime_filename(curl_mimepart_, mime_part.file_name().c_str());
        }
    }
    else {
        // raw data
        if (!mime_part.data().empty()) {
            curl_mime_data(curl_mimepart_,
                           mime_part.data().c_str(),
                           mime_part.data().length());
        }
    }
}

} // anonymous namespace

const std::string Request::curl_version_ =
    curl_version_info(CURLVERSION_NOW)->version;

Request::Request() {
    curl_global_init(CURL_GLOBAL_ALL);
    handle_ = curl_easy_init();
    useragent_ = "curl/" + curl_version_;
}

Request::~Request() {
    if (temp_stderr != nullptr) {
        std::fclose(temp_stderr);
    }
    if (curl_mime_) {
        curl_mime_free(curl_mime_);
        curl_mime_ = nullptr;
    }
    curl_easy_cleanup(handle_);
    curl_global_cleanup();
}

std::string Request::curl_version() {
    return curl_version_;
}

void Request::reset() {
    curl_easy_reset(handle_);
    method_ = request::Method::DEFAULT;
    ip_resolve_ = request::IP_Resolve::WHATEVER;
    url_.clear();
    cookie_.clear();
    header_.clear();
    useragent_ = "curl/" + curl_version_;
    authentication_.clear();
    body_.clear();
    mime_parts_.clear();
    if (curl_mime_) {
        curl_mime_free(curl_mime_);
        curl_mime_ = nullptr;
    }
    proxy_.clear();
    local_setting_.interface.clear();
    local_setting_.port = 0;
    local_setting_.port_range = 1;
    local_setting_.dns_interface.clear();
    local_setting_.dns_local_ipv4.clear();
    local_setting_.dns_local_ipv6.clear();
    timeout_setting_.timeout = 0l;
    timeout_setting_.connect_timeout = 300000l;
}

const request::Method& Request::method() const {
    return method_;
}

request::Method& Request::method() {
    return method_;
}

void Request::method(const request::Method& method) {
    method_ = method;
}

const request::IP_Resolve& Request::ip_resolve() const {
    return ip_resolve_;
}

request::IP_Resolve& Request::ip_resolve() {
    return ip_resolve_;
}

void Request::ip_resolve(const request::IP_Resolve& ip_resolve) {
    ip_resolve_ = ip_resolve;
}

const std::string& Request::url() const {
    return url_;
}

std::string& Request::url() {
    return url_;
}

void Request::url(const std::string& url) {
    url_ = url;
}

void Request::url(std::string&& url) {
    url_ = std::move(url);
}

const request::Cookie& Request::cookie() const {
    return cookie_;
}

request::Cookie& Request::cookie() {
    return cookie_;
}

void Request::cookie(const request::Cookie& cookie) {
    cookie_ = cookie;
}

void Request::cookie(request::Cookie&& cookie) {
    cookie_ = std::move(cookie);
}

const request::Header& Request::header() const {
    return header_;
}

request::Header& Request::header() {
    return header_;
}

void Request::header(const request::Header& header) {
    header_ = header;
}

void Request::header(request::Header&& header) {
    header_ = std::move(header);
}

const std::string& Request::useragent() const {
    return useragent_;
}

std::string& Request::useragent() {
    return useragent_;
}

void Request::useragent(const std::string& useragent) {
    useragent_ = useragent;
}

void Request::useragent(std::string&& useragent) {
    useragent_ = std::move(useragent);
}

const request::Authentication& Request::authentication() const {
    return authentication_;
}

request::Authentication& Request::authentication() {
    return authentication_;
}

void Request::authentication(const request::Authentication& authentication) {
    authentication_ = authentication;
}

void Request::authentication(request::Authentication&& authentication) {
    authentication_ = std::move(authentication);
}

const std::string& Request::body() const {
    return body_;
}

std::string& Request::body() {
    return body_;
}

void Request::body(const std::string& body, const std::string& mime_type) {
    body_ = body;
    header_["content-type"] = mime_type;
}

void Request::body(std::string&& body, std::string&& mime_type) {
    body_ = std::move(body);
    header_["content-type"] = std::move(mime_type);
}

request::MimeParts& Request::mime_parts() {
    return mime_parts_;
}

const request::MimeParts& Request::mime_parts() const {
    return mime_parts_;
}

void Request::mime_parts(const request::MimeParts& mime_parts) {
    mime_parts_ = mime_parts;
}

void Request::mime_parts(request::MimeParts&& mime_parts) {
    mime_parts_ = std::move(mime_parts);
}

const request::Proxy& Request::proxy() const {
    return proxy_;
}

request::Proxy& Request::proxy() {
    return proxy_;
}

void Request::proxy(const request::Proxy& proxy) {
    proxy_ = proxy;
}

void Request::proxy(request::Proxy&& proxy) {
    proxy_ = std::move(proxy);
}

const long& Request::timeout() const {
    return timeout_setting_.timeout;
}

long& Request::timeout() {
    return timeout_setting_.timeout;
}

void Request::timeout(const long& timeout) {
    timeout_setting_.timeout = timeout;
}

const long& Request::connect_timeout() const {
    return timeout_setting_.connect_timeout;
}

long& Request::connect_timeout() {
    return timeout_setting_.connect_timeout;
}

void Request::connect_timeout(const long& connect_timeout) {
    timeout_setting_.connect_timeout = connect_timeout;
}

const std::string& Request::local_interface() const {
    return local_setting_.interface;
}

std::string& Request::local_interface() {
    return local_setting_.interface;
}

void Request::local_interface(const std::string& interface) {
    local_setting_.interface = interface;
}

void Request::local_interface(std::string&& interface) {
    local_setting_.interface = std::move(interface);
}

long& Request::local_port() {
    return local_setting_.port;
}

const long& Request::local_port() const {
    return local_setting_.port;
}

void Request::local_port(const long& port) {
    local_setting_.port = port;
}

void Request::local_port(long&& port) {
    local_setting_.port = std::move(port);
}

long& Request::local_port_range() {
    return local_setting_.port_range;
}

const long& Request::local_port_range() const {
    return local_setting_.port_range;
}

void Request::local_port_range(const long& range) {
    local_setting_.port_range = range;
}

void Request::local_port_range(long&& range) {
    local_setting_.port_range = std::move(range);
}

const std::string& Request::dns_interface() const {
    return local_setting_.dns_interface;
}

std::string& Request::dns_interface() {
    return local_setting_.dns_interface;
}

void Request::dns_interface(const std::string& dns_interface) {
    local_setting_.dns_interface = dns_interface;
}

void Request::dns_interface(std::string&& dns_interface) {
    local_setting_.dns_interface = std::move(dns_interface);
}

const std::string& Request::dns_local_ipv4() const {
    return local_setting_.dns_local_ipv4;
}

std::string& Request::dns_local_ipv4() {
    return local_setting_.dns_local_ipv4;
}

void Request::dns_local_ipv4(const std::string& dns_local_ipv4) {
    local_setting_.dns_local_ipv4 = dns_local_ipv4;
}

void Request::dns_local_ipv4(std::string&& dns_local_ipv4) {
    local_setting_.dns_local_ipv4 = std::move(dns_local_ipv4);
}

const std::string& Request::dns_local_ipv6() const {
    return local_setting_.dns_local_ipv6;
}

std::string& Request::dns_local_ipv6() {
    return local_setting_.dns_local_ipv6;
}

void Request::dns_local_ipv6(const std::string& dns_local_ipv6) {
    local_setting_.dns_local_ipv6 = dns_local_ipv6;
}

void Request::dns_local_ipv6(std::string&& dns_local_ipv6) {
    local_setting_.dns_local_ipv6 = std::move(dns_local_ipv6);
}

bool Request::request() {
    init_curl_handle_();
    set_options_();
    return perform_request_();
}

const request::Response& Request::response() const {
    return response_;
}

request::Response& Request::response() {
    return response_;
}

std::string Request::url_encode(const std::string& url) {
    char* result_c_str = curl_easy_escape(handle_, url.c_str(), url.length());
    std::string result = result_c_str;
    curl_free(result_c_str);
    return result;
}

std::string Request::url_encode(const char* url, std::size_t len) {
    char* result_c_str = curl_easy_escape(handle_, url, len);
    std::string result = result_c_str;
    curl_free(result_c_str);
    return result;
}

std::string Request::url_decode(const std::string& url) {
    int result_length;
    char* result_c_str =
        curl_easy_unescape(handle_, url.c_str(), url.length(), &result_length);
    std::string result(result_c_str, result_length);
    curl_free(result_c_str);
    return result;
}

std::string Request::url_decode(const char* url, std::size_t len) {
    int result_length;
    char* result_c_str = curl_easy_unescape(handle_, url, len, &result_length);
    std::string result(result_c_str, result_length);
    curl_free(result_c_str);
    return result;
}

void Request::init_curl_handle_() {
    // useful for multithreading??
    curl_easy_setopt(handle_, CURLOPT_NOSIGNAL, 1l);
    // enable TCP keep-alive
    curl_easy_setopt(handle_, CURLOPT_TCP_KEEPALIVE, 1l);
    // get verbose message
    curl_easy_setopt(handle_, CURLOPT_VERBOSE, 1l);
    // receive cookie
    curl_easy_setopt(handle_, CURLOPT_COOKIEFILE, "");
    // receive cert info
    curl_easy_setopt(handle_, CURLOPT_CERTINFO, 1l);
    // receive error
    error_info_buffer[0] = 0;
    curl_easy_setopt(handle_, CURLOPT_ERRORBUFFER, error_info_buffer);
    // receive response text
    curl_easy_setopt(
        handle_, CURLOPT_WRITEFUNCTION, curl_custom_write_function);
    curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &response_.body);
    // receive response header
    curl_easy_setopt(
        handle_, CURLOPT_HEADERFUNCTION, curl_custom_header_function);
    curl_easy_setopt(handle_, CURLOPT_HEADERDATA, &response_.header);
    // verbose information
    if (temp_stderr == nullptr) {
        temp_stderr = std::tmpfile();
    }
    curl_easy_setopt(handle_, CURLOPT_STDERR, temp_stderr);
}

void Request::set_options_() {
    // [option] method
    curl_easy_setopt(
        handle_, CURLOPT_CUSTOMREQUEST, request::method::c_str(method_));
    // [option] ip resolve
    curl_easy_setopt(handle_,
                     CURLOPT_IPRESOLVE,
                     request::ip_resolve::as_option(ip_resolve_));
    // [option] url
    if (!url_.empty()) {
        curl_easy_setopt(handle_, CURLOPT_URL, url_.c_str());
    }
    // [option] cookie
    if (!cookie_.empty()) {
        curl_easy_setopt(handle_, CURLOPT_COOKIE, cookie_.c_str());
    }
    // [option] header
    if (!header_.empty()) {
        curl_easy_setopt(handle_, CURLOPT_HTTPHEADER, header_.curl_header());
    }
    // [option] useragent
    if (!useragent_.empty()) {
        curl_easy_setopt(handle_, CURLOPT_USERAGENT, useragent_.c_str());
    }
    // [option] authentication
    if (!authentication_.empty()) {
        curl_easy_setopt(handle_, CURLOPT_USERPWD, authentication_.c_str());
    }
    // [option] body
    if (!body_.empty()) {
        curl_easy_setopt(handle_, CURLOPT_POSTFIELDS, body_.c_str());
        curl_easy_setopt(handle_, CURLOPT_POSTFIELDSIZE_LARGE, body_.length());
    }
    // [option] multi part
    if (!mime_parts_.empty()) {
        // clean previous curl_mime
        if (curl_mime_) {
            curl_mime_free(curl_mime_);
            curl_mime_ = nullptr;
        }
        // init fresh curl_mime
        curl_mime_ = curl_mime_init(handle_);
        // add parts
        for (const std::pair<std::string, request::MimePart>& mime_part :
             mime_parts_) {
            curl_add_mime_part(curl_mime_, mime_part.first, mime_part.second);
        }
        curl_easy_setopt(handle_, CURLOPT_MIMEPOST, curl_mime_);
    }
    // [option] proxy
    if (!proxy_.empty()) {
        curl_easy_setopt(handle_, CURLOPT_PROXY, proxy_.c_str());
    }
    // [option] http proxy tunnel
    if (proxy_.http_proxy_tunnel()) {
        curl_easy_setopt(handle_, CURLOPT_HTTPPROXYTUNNEL, 1l);
    }
    // [option] timeout
    if (timeout_setting_.timeout) {
        curl_easy_setopt(handle_,
                         CURLOPT_TIMEOUT_MS,
                         static_cast<curl_off_t>(timeout_setting_.timeout));
    }
    // [option] connect timeout
    if (timeout_setting_.connect_timeout != 300000l) {
        curl_easy_setopt(
            handle_,
            CURLOPT_CONNECTTIMEOUT_MS,
            static_cast<curl_off_t>(timeout_setting_.connect_timeout));
    }
    // [option] local interface
    if (!local_setting_.interface.empty()) {
        curl_easy_setopt(
            handle_, CURLOPT_INTERFACE, local_setting_.interface.c_str());
    }
    // [option] local port
    if (local_setting_.port != 0) {
        curl_easy_setopt(handle_, CURLOPT_LOCALPORT, &local_setting_.port);
    }
    // [option] local port range
    if (local_setting_.port_range != 1) {
        curl_easy_setopt(
            handle_, CURLOPT_LOCALPORTRANGE, &local_setting_.port_range);
    }
    // [option] dns interface
    if (!local_setting_.dns_interface.empty()) {
        curl_easy_setopt(handle_,
                         CURLOPT_DNS_INTERFACE,
                         local_setting_.dns_interface.c_str());
    }
    // [option] dns local ipv4 address
    if (!local_setting_.dns_local_ipv4.empty()) {
        curl_easy_setopt(handle_,
                         CURLOPT_DNS_LOCAL_IP4,
                         local_setting_.dns_local_ipv4.c_str());
    }
    // [option] dns local ipv6 address
    if (!local_setting_.dns_local_ipv6.empty()) {
        curl_easy_setopt(handle_,
                         CURLOPT_DNS_LOCAL_IP6,
                         local_setting_.dns_local_ipv6.c_str());
    }
}

bool Request::perform_request_() {
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
    std::rewind(temp_stderr);
    char temp_buffer[64];
    while (std::fgets(temp_buffer, 64, temp_stderr) != nullptr) {
        response_.verbose.append(temp_buffer);
    }
    return (curl_result == CURLE_OK);
}

namespace request {

std::time_t parse_time_str(const char* time_str) {
    return curl_getdate(time_str, nullptr);
}

} // namespace request

} // namespace utils
} // namespace rayalto
