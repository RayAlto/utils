#include "request/request.h"

#include <cstddef>
#include <cstdio>
#include <cstring> // std::strlen, std::size_t
#include <ctime>
#include <memory>
#include <string>
#include <utility>

#include "curl/curl.h"

#include "request/cookie.h"
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

void init_curl_header(const request::Header& headers,
                      curl_slist** curl_header) {
    if (*curl_header != nullptr) {
        curl_slist_free_all(*curl_header);
        *curl_header = nullptr;
    }
    if (headers.empty()) {
        return;
    }
    std::string header_line;
    for (const std::pair<std::string, std::string>& header : headers) {
        header_line = header.first + ": " + header.second;
        *curl_header = curl_slist_append(*curl_header, header_line.c_str());
    }
}

void parse_cookie_slist(curl_slist* curl_cookies, request::Cookie& cookie) {
    if (!curl_cookies) {
        return;
    }
    for (curl_slist* curl_cookie = curl_cookies; curl_cookie;
         curl_cookie = curl_cookie->next) {
        std::vector<std::string> parts = string::split(curl_cookie->data, '\t');
        cookie[parts[5]] = parts[6];
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
    useragent_ = std::make_unique<std::string>("curl/" + curl_version_);
}

Request::~Request() {
    if (temp_stderr_ != nullptr) {
        std::fclose(temp_stderr_);
    }
    if (curl_mime_ != nullptr) {
        curl_mime_free(curl_mime_);
        curl_mime_ = nullptr;
    }
    if (curl_header_ != nullptr) {
        curl_slist_free_all(curl_header_);
    }
    curl_easy_cleanup(handle_);
    curl_global_cleanup();
}

std::string Request::curl_version() {
    return curl_version_;
}

void Request::reset() {
    curl_easy_reset(handle_);
    if (temp_stderr_ != nullptr) {
        std::fclose(temp_stderr_);
        temp_stderr_ = std::tmpfile();
    }
    method_ = request::Method::DEFAULT;
    ip_resolve_ = request::IpResolve::WHATEVER;
    url_.reset();
    cookie_.reset();
    header_.reset();
    useragent_ = std::make_unique<std::string>("curl/" + curl_version_);
    authentication_.reset();
    body_.reset();
    mime_parts_.reset();
    proxy_.reset();
    local_setting_.reset();
    timeout_setting_.reset();
    response_.reset();
    if (curl_mime_) {
        curl_mime_free(curl_mime_);
        curl_mime_ = nullptr;
    }
    if (curl_header_ != nullptr) {
        curl_slist_free_all(curl_header_);
    }
}

request::Method Request::method() {
    return method_;
}

Request& Request::method(request::Method method) {
    method_ = method;
    return *this;
}

request::IpResolve Request::ip_resolve() {
    return ip_resolve_;
}

Request& Request::ip_resolve(request::IpResolve ip_resolve) {
    ip_resolve_ = ip_resolve;
    return *this;
}

std::string Request::url() {
    if (url_ == nullptr) {
        url_ = std::make_unique<std::string>();
    }
    return *url_;
}

Request& Request::url(const std::string& url) {
    url_ = std::make_unique<std::string>(url);
    return *this;
}

Request& Request::url(std::string&& url) {
    url_ = std::make_unique<std::string>(std::move(url));
    return *this;
}

request::Cookie Request::cookie() {
    if (cookie_ == nullptr) {
        cookie_ = std::make_unique<request::Cookie>();
    }
    return *cookie_;
}

Request& Request::cookie(const request::Cookie& cookie) {
    cookie_ = std::make_unique<request::Cookie>(cookie);
    return *this;
}

Request& Request::cookie(request::Cookie&& cookie) {
    cookie_ = std::make_unique<request::Cookie>(std::move(cookie));
    return *this;
}

request::Header Request::header() {
    if (header_ == nullptr) {
        header_ = std::make_unique<request::Header>();
    }
    return *header_;
}

Request& Request::header(const request::Header& header) {
    header_ = std::make_unique<request::Header>(header);
    return *this;
}

Request& Request::header(request::Header&& header) {
    header_ = std::make_unique<request::Header>(std::move(header));
    return *this;
}

std::string Request::useragent() {
    if (useragent_ == nullptr) {
        useragent_ = std::make_unique<std::string>();
    }
    return *useragent_;
}

Request& Request::useragent(const std::string& useragent) {
    useragent_ = std::make_unique<std::string>(useragent);
    return *this;
}

Request& Request::useragent(std::string&& useragent) {
    useragent_ = std::make_unique<std::string>(std::move(useragent));
    return *this;
}

request::Authentication Request::authentication() {
    if (authentication_ == nullptr) {
        authentication_ = std::make_unique<request::Authentication>();
    }
    return *authentication_;
}

Request& Request::authentication(
    const request::Authentication& authentication) {
    authentication_ = std::make_unique<request::Authentication>(authentication);
    return *this;
}

Request& Request::authentication(request::Authentication&& authentication) {
    authentication_ =
        std::make_unique<request::Authentication>(std::move(authentication));
    return *this;
}

std::string Request::body() {
    if (body_ == nullptr) {
        body_ = std::make_unique<std::string>();
    }
    return *body_;
}

Request& Request::body(const std::string& body, const std::string& mime_type) {
    body_ = std::make_unique<std::string>(body);
    if (header_ == nullptr) {
        header_ = std::make_unique<request::Header>();
    }
    (*header_)["content-type"] = mime_type;
    return *this;
}

Request& Request::body(std::string&& body, std::string&& mime_type) {
    body_ = std::make_unique<std::string>(std::move(body));
    if (header_ == nullptr) {
        header_ = std::make_unique<request::Header>();
    }
    (*header_)["content-type"] = std::move(mime_type);
    return *this;
}

request::MimeParts Request::mime_parts() {
    if (mime_parts_ == nullptr) {
        mime_parts_ = std::make_unique<request::MimeParts>();
    }
    return *mime_parts_;
}

Request& Request::mime_parts(const request::MimeParts& mime_parts) {
    mime_parts_ = std::make_unique<request::MimeParts>(mime_parts);
    return *this;
}

Request& Request::mime_parts(request::MimeParts&& mime_parts) {
    mime_parts_ = std::make_unique<request::MimeParts>(std::move(mime_parts));
    return *this;
}

request::Proxy Request::proxy() {
    if (proxy_ == nullptr) {
        proxy_ = std::make_unique<request::Proxy>();
    }
    return *proxy_;
}

Request& Request::proxy(const request::Proxy& proxy) {
    proxy_ = std::make_unique<request::Proxy>(proxy);
    return *this;
}

Request& Request::proxy(request::Proxy&& proxy) {
    proxy_ = std::make_unique<request::Proxy>(std::move(proxy));
    return *this;
}

long Request::timeout() {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    return timeout_setting_->timeout;
}

Request& Request::timeout(const long& timeout) {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    timeout_setting_->timeout = timeout;
    return *this;
}

Request& Request::timeout(long&& timeout) {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    timeout_setting_->timeout = std::move(timeout);
    return *this;
}

long Request::connect_timeout() {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    return timeout_setting_->connect_timeout;
}

Request& Request::connect_timeout(const long& connect_timeout) {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    timeout_setting_->connect_timeout = connect_timeout;
    return *this;
}

Request& Request::connect_timeout(long&& connect_timeout) {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    timeout_setting_->connect_timeout = std::move(connect_timeout);
    return *this;
}

std::string Request::local_interface() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->interface;
}

Request& Request::local_interface(const std::string& local_interface) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->interface = local_interface;
    return *this;
}

Request& Request::local_interface(std::string&& local_interface) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->interface = std::move(local_interface);
    return *this;
}

long Request::local_port() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->port;
}

Request& Request::local_port(const long& local_port) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->port = local_port;
    return *this;
}

Request& Request::local_port(long&& local_port) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->port = std::move(local_port);
    return *this;
}

long Request::local_port_range() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->port_range;
}

Request& Request::local_port_range(const long& local_port_range) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->port_range = local_port_range;
    return *this;
}

Request& Request::local_port_range(long&& local_port_range) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->port_range = std::move(local_port_range);
    return *this;
}

std::string Request::dns_interface() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->dns_interface;
}

Request& Request::dns_interface(const std::string& dns_interface) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_interface = dns_interface;
    return *this;
}

Request& Request::dns_interface(std::string&& dns_interface) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_interface = std::move(dns_interface);
    return *this;
}

std::string Request::dns_local_ipv4() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->dns_local_ipv4;
}

Request& Request::dns_local_ipv4(const std::string& dns_local_ipv4) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_local_ipv4 = dns_local_ipv4;
    return *this;
}

Request& Request::dns_local_ipv4(std::string&& dns_local_ipv4) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_local_ipv4 = std::move(dns_local_ipv4);
    return *this;
}

std::string Request::dns_local_ipv6() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->dns_local_ipv6;
}

Request& Request::dns_local_ipv6(const std::string& dns_local_ipv6) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_local_ipv6 = dns_local_ipv6;
    return *this;
}

Request& Request::dns_local_ipv6(std::string&& dns_local_ipv6) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_local_ipv6 = std::move(dns_local_ipv6);
    return *this;
}

bool Request::request() {
    init_curl_handle_();
    set_options_();
    return perform_request_();
}

request::Response Request::response() {
    if (response_ == nullptr) {
        response_ = std::make_unique<request::Response>();
    }
    return *response_;
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
    error_info_buffer_[0] = 0;
    curl_easy_setopt(handle_, CURLOPT_ERRORBUFFER, error_info_buffer_);
    // receive response text
    curl_easy_setopt(
        handle_, CURLOPT_WRITEFUNCTION, curl_custom_write_function);
    curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &response_->body);
    // receive response header
    curl_easy_setopt(
        handle_, CURLOPT_HEADERFUNCTION, curl_custom_header_function);
    curl_easy_setopt(handle_, CURLOPT_HEADERDATA, &response_->header);
    // verbose information
    if (temp_stderr_ == nullptr) {
        temp_stderr_ = std::tmpfile();
    }
    curl_easy_setopt(handle_, CURLOPT_STDERR, temp_stderr_);
    init_curl_header(*header_, &curl_header_);
}

void Request::set_options_() {
    // [option] method
    curl_easy_setopt(
        handle_, CURLOPT_CUSTOMREQUEST, request::method::c_str(method_));
    // [option] ip resolve
    curl_easy_setopt(
        handle_,
        CURLOPT_IPRESOLVE,
        (ip_resolve_ == request::IpResolve::IPv4_ONLY ? CURL_IPRESOLVE_V4
         : ip_resolve_ == request::IpResolve::IPv6_ONLY
             ? CURL_IPRESOLVE_V6
             : CURL_IPRESOLVE_WHATEVER));
    // [option] url
    if (url_ != nullptr) {
        curl_easy_setopt(handle_, CURLOPT_URL, url_->c_str());
    }
    // [option] cookie
    if (cookie_ != nullptr) {
        curl_easy_setopt(handle_, CURLOPT_COOKIE, cookie_->c_str());
    }
    // [option] header
    if (curl_header_ != nullptr) {
        curl_easy_setopt(handle_, CURLOPT_HTTPHEADER, curl_header_);
    }
    // [option] useragent
    if (useragent_ != nullptr) {
        curl_easy_setopt(handle_, CURLOPT_USERAGENT, useragent_->c_str());
    }
    // [option] authentication
    if (authentication_ != nullptr) {
        curl_easy_setopt(handle_, CURLOPT_USERPWD, authentication_->c_str());
    }
    // [option] body
    if (body_ != nullptr) {
        curl_easy_setopt(handle_, CURLOPT_POSTFIELDS, body_->c_str());
        curl_easy_setopt(handle_, CURLOPT_POSTFIELDSIZE_LARGE, body_->length());
    }
    // [option] multi part
    if (mime_parts_ != nullptr) {
        // clean previous curl_mime
        if (curl_mime_) {
            curl_mime_free(curl_mime_);
            curl_mime_ = nullptr;
        }
        // init fresh curl_mime
        curl_mime_ = curl_mime_init(handle_);
        // add parts
        for (const std::pair<std::string, request::MimePart>& mime_part :
             *mime_parts_) {
            curl_add_mime_part(curl_mime_, mime_part.first, mime_part.second);
        }
        curl_easy_setopt(handle_, CURLOPT_MIMEPOST, curl_mime_);
    }
    // [option] proxy
    if (proxy_ != nullptr) {
        curl_easy_setopt(handle_, CURLOPT_PROXY, proxy_->c_str());
        // [option] http proxy tunnel
        if (proxy_->http_proxy_tunnel()) {
            curl_easy_setopt(handle_, CURLOPT_HTTPPROXYTUNNEL, 1l);
        }
    }
    if (timeout_setting_ != nullptr) {
        // [option] timeout
        if (timeout_setting_->timeout != 0l) {
            curl_easy_setopt(
                handle_,
                CURLOPT_TIMEOUT_MS,
                static_cast<curl_off_t>(timeout_setting_->timeout));
        }
        // [option] connect timeout
        if (timeout_setting_->connect_timeout != 300000l) {
            curl_easy_setopt(
                handle_,
                CURLOPT_CONNECTTIMEOUT_MS,
                static_cast<curl_off_t>(timeout_setting_->connect_timeout));
        }
    }
    if (local_setting_ != nullptr) {
        // [option] local interface
        if (!local_setting_->interface.empty()) {
            curl_easy_setopt(
                handle_, CURLOPT_INTERFACE, local_setting_->interface.c_str());
        }
        // [option] local port
        if (local_setting_->port != 0) {
            curl_easy_setopt(handle_, CURLOPT_LOCALPORT, &local_setting_->port);
        }
        // [option] local port range
        if (local_setting_->port_range != 1) {
            curl_easy_setopt(
                handle_, CURLOPT_LOCALPORTRANGE, &local_setting_->port_range);
        }
        // [option] dns interface
        if (!local_setting_->dns_interface.empty()) {
            curl_easy_setopt(handle_,
                             CURLOPT_DNS_INTERFACE,
                             local_setting_->dns_interface.c_str());
        }
        // [option] dns local ipv4 address
        if (!local_setting_->dns_local_ipv4.empty()) {
            curl_easy_setopt(handle_,
                             CURLOPT_DNS_LOCAL_IP4,
                             local_setting_->dns_local_ipv4.c_str());
        }
        // [option] dns local ipv6 address
        if (!local_setting_->dns_local_ipv6.empty()) {
            curl_easy_setopt(handle_,
                             CURLOPT_DNS_LOCAL_IP6,
                             local_setting_->dns_local_ipv6.c_str());
        }
    }
}

bool Request::perform_request_() {
    // perform
    CURLcode curl_result = curl_easy_perform(handle_);
    // receive message
    response_ = std::make_unique<request::Response>();
    response_->message = (std::strlen(error_info_buffer_) == 0)
                             ? error_info_buffer_
                             : curl_easy_strerror(curl_result);
    // receive response code
    curl_easy_getinfo(handle_, CURLINFO_RESPONSE_CODE, &response_->code);
    // receive response cookie
    curl_slist* curl_cookies = nullptr;
    curl_easy_getinfo(handle_, CURLINFO_COOKIELIST, &curl_cookies);
    parse_cookie_slist(curl_cookies, response_->cookie);
    curl_slist_free_all(curl_cookies);
    // receive connection http version
    curl_easy_getinfo(handle_, CURLINFO_HTTP_VERSION, &response_->http_version);
    // receive time elapsed
    curl_easy_getinfo(
        handle_, CURLINFO_TOTAL_TIME, &response_->time_elapsed.all);
    curl_easy_getinfo(handle_,
                      CURLINFO_NAMELOOKUP_TIME,
                      &response_->time_elapsed.name_resolve);
    curl_easy_getinfo(
        handle_, CURLINFO_CONNECT_TIME, &response_->time_elapsed.connect);
    curl_easy_getinfo(
        handle_, CURLINFO_APPCONNECT_TIME, &response_->time_elapsed.handshake);
    curl_easy_getinfo(handle_,
                      CURLINFO_PRETRANSFER_TIME,
                      &response_->time_elapsed.pre_transfer);
    curl_easy_getinfo(handle_,
                      CURLINFO_STARTTRANSFER_TIME,
                      &response_->time_elapsed.start_transfer);
    curl_easy_getinfo(
        handle_, CURLINFO_SIZE_UPLOAD_T, &response_->byte_transfered.upload);
    curl_easy_getinfo(handle_,
                      CURLINFO_SIZE_DOWNLOAD_T,
                      &response_->byte_transfered.download);
    curl_easy_getinfo(
        handle_, CURLINFO_SPEED_UPLOAD_T, &response_->speed.upload);
    curl_easy_getinfo(
        handle_, CURLINFO_SPEED_DOWNLOAD_T, &response_->speed.download);
    char* local_ip;
    curl_easy_getinfo(handle_, CURLINFO_LOCAL_IP, &local_ip);
    response_->local_info.ip = local_ip;
    curl_easy_getinfo(
        handle_, CURLINFO_LOCAL_PORT, &response_->local_info.port);
    std::rewind(temp_stderr_);
    char temp_buffer[64];
    while (std::fgets(temp_buffer, 64, temp_stderr_) != nullptr) {
        response_->verbose.append(temp_buffer);
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
