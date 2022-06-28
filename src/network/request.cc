#include "rautils/network/request.h"

#include <cstddef>
#include <cstdio>
#include <cstring> // std::strlen, std::size_t
#include <ctime>
#include <memory>
#include <string>
#include <utility>

#include "curl/curl.h"

#include "rautils/network/general/authentication.h"
#include "rautils/network/general/cookie.h"
#include "rautils/network/general/header.h"
#include "rautils/network/general/url.h"
#include "rautils/string/strtool.h"

namespace rayalto {
namespace utils {
namespace network {

namespace {

void parse_header(const char* data,
                  const std::size_t& size,
                  general::Header& result) {
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

void init_curl_header(const std::unique_ptr<general::Header>& headers,
                      curl_slist** curl_header) {
    if (*curl_header != nullptr) {
        curl_slist_free_all(*curl_header);
        *curl_header = nullptr;
    }
    if (headers == nullptr) {
        return;
    }
    std::string header_line;
    for (const std::pair<std::string, std::string>& header : *headers) {
        header_line = header.first + ": " + header.second;
        *curl_header = curl_slist_append(*curl_header, header_line.c_str());
    }
}

void parse_cookie_slist(curl_slist* curl_cookies, general::Cookie& cookie) {
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
                                        general::Header* userdata) {
    std::size_t actual_size = size * nitems;
    parse_header(buffer, actual_size, *userdata);
    return actual_size;
}

void curl_add_mime_part(curl_mime* mime,
                        const std::string& part_name,
                        const Request::MimePart& mime_part) {
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

class Request::Impl {
public:
    Impl();
    Impl(const Impl&) = delete;
    Impl(Impl&&) noexcept = default;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&) noexcept = default;

    virtual ~Impl();

    static const std::string& curl_version();

    void reset();

    const Method& method() const;
    Method& method();
    Impl& method(Method method);

    const IpResolve& ip_resolve() const;
    IpResolve& ip_resolve();
    Impl& ip_resolve(IpResolve ip_resolve);

    const std::unique_ptr<general::Url>& url();
    Impl& url(const general::Url& url);
    Impl& url(general::Url&& url);

    const std::unique_ptr<general::Cookie>& cookie();
    Impl& cookie(const general::Cookie& cookie);
    Impl& cookie(general::Cookie&& cookie);

    const std::unique_ptr<general::Header>& header();
    Impl& header(const general::Header& header);
    Impl& header(general::Header&& header);

    const std::unique_ptr<std::string>& useragent();
    Impl& useragent(const std::string& useragent);
    Impl& useragent(std::string&& useragent);

    const std::unique_ptr<general::Authentication>& authentication();
    Impl& authentication(const general::Authentication& authentication);
    Impl& authentication(general::Authentication&& authentication);

    const std::unique_ptr<std::string>& body();
    Impl& body(
        const std::string& body,
        const std::string& mime_type = "application/x-www-form-urlencoded");
    Impl& body(std::string&& body,
               std::string&& mime_type = "application/x-www-form-urlencoded");

    const std::unique_ptr<MimeParts>& mime_parts();
    Impl& mime_parts(const MimeParts& mime_parts);
    Impl& mime_parts(MimeParts&& mime_parts);

    const std::unique_ptr<Proxy>& proxy();
    Impl& proxy(const Proxy& proxy);
    Impl& proxy(Proxy&& proxy);

    const std::unique_ptr<TimeoutSetting>& timeout_setting();
    Impl& timeout_setting(const TimeoutSetting& timeout);
    Impl& timeout_setting(TimeoutSetting&& timeout);

    const std::unique_ptr<LocalSetting>& local_setting();
    Impl& local_setting(const LocalSetting& local);
    Impl& local_setting(LocalSetting&& local);

    bool request();

    const std::unique_ptr<Response>& response();

    /* ===== other func ===== */
    std::string url_encode(const std::string& url);
    std::string url_encode(const char* url, std::size_t len = 0);
    std::string url_decode(const std::string& url);
    std::string url_decode(const char* url, std::size_t len = 0);

protected:
    CURL* handle_;

    char error_info_buffer_[CURL_ERROR_SIZE];
    std::FILE* temp_stderr_ = nullptr;

    Method method_ = Method::DEFAULT;
    IpResolve ip_resolve_ = IpResolve::WHATEVER;
    std::unique_ptr<general::Url> url_ = nullptr;
    std::unique_ptr<general::Cookie> cookie_ = nullptr;
    std::unique_ptr<general::Header> header_ = nullptr;
    std::unique_ptr<std::string> useragent_ = nullptr;
    std::unique_ptr<general::Authentication> authentication_ = nullptr;
    std::unique_ptr<std::string> body_ = nullptr;
    std::unique_ptr<MimeParts> mime_parts_ = nullptr;
    std::unique_ptr<Proxy> proxy_ = nullptr;
    std::unique_ptr<TimeoutSetting> timeout_setting_ = nullptr;
    std::unique_ptr<LocalSetting> local_setting_ = nullptr;
    std::unique_ptr<Response> response_ = nullptr;

    // why the FUCK curl_mime_init() need a curl handle?
    curl_mime* curl_mime_ = nullptr;
    curl_slist* curl_header_ = nullptr;

    const static std::string curl_version_;

    void init_curl_handle_();
    void set_options_();
    bool perform_request_();
};

const std::string Request::Impl::curl_version_ =
    curl_version_info(CURLVERSION_NOW)->version;

Request::Impl::Impl() {
    curl_global_init(CURL_GLOBAL_ALL);
    handle_ = curl_easy_init();
    useragent_ = std::make_unique<std::string>("curl/" + curl_version_);
}

Request::Impl::~Impl() {
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

const std::string& Request::Impl::curl_version() {
    return curl_version_;
}

void Request::Impl::reset() {
    curl_easy_reset(handle_);
    if (temp_stderr_ != nullptr) {
        std::fclose(temp_stderr_);
        temp_stderr_ = std::tmpfile();
    }
    method_ = Method::DEFAULT;
    ip_resolve_ = IpResolve::WHATEVER;
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

const Request::Method& Request::Impl::method() const {
    return method_;
}

Request::Method& Request::Impl::method() {
    return method_;
}

Request::Impl& Request::Impl::method(Method method) {
    method_ = method;
    return *this;
}

const Request::IpResolve& Request::Impl::ip_resolve() const {
    return ip_resolve_;
}

Request::IpResolve& Request::Impl::ip_resolve() {
    return ip_resolve_;
}

Request::Impl& Request::Impl::ip_resolve(IpResolve ip_resolve) {
    ip_resolve_ = ip_resolve;
    return *this;
}

const std::unique_ptr<general::Url>& Request::Impl::url() {
    return url_;
}

Request::Impl& Request::Impl::url(const general::Url& url) {
    url_ = std::make_unique<general::Url>(url);
    return *this;
}

Request::Impl& Request::Impl::url(general::Url&& url) {
    url_ = std::make_unique<general::Url>(std::move(url));
    return *this;
}

const std::unique_ptr<general::Cookie>& Request::Impl::cookie() {
    return cookie_;
}

Request::Impl& Request::Impl::cookie(const general::Cookie& cookie) {
    cookie_ = std::make_unique<general::Cookie>(cookie);
    return *this;
}

Request::Impl& Request::Impl::cookie(general::Cookie&& cookie) {
    cookie_ = std::make_unique<general::Cookie>(std::move(cookie));
    return *this;
}

const std::unique_ptr<general::Header>& Request::Impl::header() {
    return header_;
}

Request::Impl& Request::Impl::header(const general::Header& header) {
    header_ = std::make_unique<general::Header>(header);
    return *this;
}

Request::Impl& Request::Impl::header(general::Header&& header) {
    header_ = std::make_unique<general::Header>(std::move(header));
    return *this;
}

const std::unique_ptr<std::string>& Request::Impl::useragent() {
    return useragent_;
}

Request::Impl& Request::Impl::useragent(const std::string& useragent) {
    useragent_ = std::make_unique<std::string>(useragent);
    return *this;
}

Request::Impl& Request::Impl::useragent(std::string&& useragent) {
    useragent_ = std::make_unique<std::string>(std::move(useragent));
    return *this;
}

const std::unique_ptr<general::Authentication>&
Request::Impl::authentication() {
    return authentication_;
}

Request::Impl& Request::Impl::authentication(
    const general::Authentication& authentication) {
    authentication_ = std::make_unique<general::Authentication>(authentication);
    return *this;
}

Request::Impl& Request::Impl::authentication(
    general::Authentication&& authentication) {
    authentication_ =
        std::make_unique<general::Authentication>(std::move(authentication));
    return *this;
}

const std::unique_ptr<std::string>& Request::Impl::body() {
    return body_;
}

Request::Impl& Request::Impl::body(const std::string& body,
                                   const std::string& mime_type) {
    body_ = std::make_unique<std::string>(body);
    if (header_ == nullptr) {
        header_ = std::make_unique<general::Header>();
    }
    (*header_)["content-type"] = mime_type;
    return *this;
}

Request::Impl& Request::Impl::body(std::string&& body,
                                   std::string&& mime_type) {
    body_ = std::make_unique<std::string>(std::move(body));
    if (header_ == nullptr) {
        header_ = std::make_unique<general::Header>();
    }
    (*header_)["content-type"] = std::move(mime_type);
    return *this;
}

const std::unique_ptr<Request::MimeParts>& Request::Impl::mime_parts() {
    return mime_parts_;
}

Request::Impl& Request::Impl::mime_parts(const MimeParts& mime_parts) {
    mime_parts_ = std::make_unique<MimeParts>(mime_parts);
    return *this;
}

Request::Impl& Request::Impl::mime_parts(MimeParts&& mime_parts) {
    mime_parts_ = std::make_unique<MimeParts>(std::move(mime_parts));
    return *this;
}

const std::unique_ptr<Request::Proxy>& Request::Impl::proxy() {
    return proxy_;
}

Request::Impl& Request::Impl::proxy(const Proxy& proxy) {
    proxy_ = std::make_unique<Proxy>(proxy);
    return *this;
}

Request::Impl& Request::Impl::proxy(Proxy&& proxy) {
    proxy_ = std::make_unique<Proxy>(std::move(proxy));
    return *this;
}

const std::unique_ptr<Request::TimeoutSetting>&
Request::Impl::timeout_setting() {
    return timeout_setting_;
}

Request::Impl& Request::Impl::timeout_setting(const TimeoutSetting& timeout) {
    timeout_setting_ = std::make_unique<TimeoutSetting>(timeout);
    return *this;
}

Request::Impl& Request::Impl::timeout_setting(TimeoutSetting&& timeout) {
    timeout_setting_ = std::make_unique<TimeoutSetting>(std::move(timeout));
    return *this;
}

const std::unique_ptr<Request::LocalSetting>& Request::Impl::local_setting() {
    return local_setting_;
}

Request::Impl& Request::Impl::local_setting(const LocalSetting& local) {
    local_setting_ = std::make_unique<LocalSetting>(local);
    return *this;
}

Request::Impl& Request::Impl::local_setting(LocalSetting&& local) {
    local_setting_ = std::make_unique<LocalSetting>(std::move(local));
    return *this;
}

bool Request::Impl::request() {
    init_curl_handle_();
    set_options_();
    return perform_request_();
}

const std::unique_ptr<Request::Response>& Request::Impl::response() {
    return response_;
}

std::string Request::Impl::url_encode(const std::string& url) {
    char* result_c_str = curl_easy_escape(handle_, url.c_str(), url.length());
    std::string result = result_c_str;
    curl_free(result_c_str);
    return result;
}

std::string Request::Impl::url_encode(const char* url, std::size_t len) {
    char* result_c_str = curl_easy_escape(handle_, url, len);
    std::string result = result_c_str;
    curl_free(result_c_str);
    return result;
}

std::string Request::Impl::url_decode(const std::string& url) {
    int result_length;
    char* result_c_str =
        curl_easy_unescape(handle_, url.c_str(), url.length(), &result_length);
    std::string result(result_c_str, result_length);
    curl_free(result_c_str);
    return result;
}

std::string Request::Impl::url_decode(const char* url, std::size_t len) {
    int result_length;
    char* result_c_str = curl_easy_unescape(handle_, url, len, &result_length);
    std::string result(result_c_str, result_length);
    curl_free(result_c_str);
    return result;
}

void Request::Impl::init_curl_handle_() {
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
    // allocate response
    if (response_ == nullptr) {
        response_ = std::make_unique<Response>();
    }
    curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &response_->body);
    // receive response header
    curl_easy_setopt(
        handle_, CURLOPT_HEADERFUNCTION, curl_custom_header_function);
    curl_easy_setopt(handle_, CURLOPT_HEADERDATA, &response_->header);
    // init FILE*
    if (temp_stderr_ == nullptr) {
        temp_stderr_ = std::tmpfile();
    }
    // verbose information
    curl_easy_setopt(handle_, CURLOPT_STDERR, temp_stderr_);
    init_curl_header(header_, &curl_header_);
}

void Request::Impl::set_options_() {
    // [option] method
    curl_easy_setopt(handle_, CURLOPT_CUSTOMREQUEST, method_c_str(method_));
    // [option] ip resolve
    curl_easy_setopt(
        handle_,
        CURLOPT_IPRESOLVE,
        (ip_resolve_ == IpResolve::IPv4_ONLY   ? CURL_IPRESOLVE_V4
         : ip_resolve_ == IpResolve::IPv6_ONLY ? CURL_IPRESOLVE_V6
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
        for (const std::pair<std::string, MimePart>& mime_part : *mime_parts_) {
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

bool Request::Impl::perform_request_() {
    // perform
    CURLcode curl_result = curl_easy_perform(handle_);
    // init response
    if (response_ == nullptr) {
        response_ = std::make_unique<Response>();
    }
    // receive message
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

constexpr const char* Request::method_c_str(const Method& method) {
    return method == Method::GET      ? "GET"
           : method == Method::POST   ? "POST"
           : method == Method::PUT    ? "PUT"
           : method == Method::DELETE ? "DELETE"
           : method == Method::PATCH  ? "PATCH"
                                      : nullptr;
}

Request::Request() : impl_(std::make_unique<Impl>()) {}

Request::~Request() = default;

std::string Request::curl_version() {
    return Impl::curl_version();
}

void Request::reset() {
    impl_->reset();
}

const Request::Method& Request::method() const {
    return impl_->method();
}

Request::Method& Request::method() {
    return impl_->method();
}

Request& Request::method(Method method) {
    impl_->method(method);
    return *this;
}

const Request::IpResolve& Request::ip_resolve() const {
    return impl_->ip_resolve();
}

Request::IpResolve& Request::ip_resolve() {
    return impl_->ip_resolve();
}

Request& Request::ip_resolve(IpResolve ip_resolve) {
    impl_->ip_resolve(ip_resolve);
    return *this;
}

const std::unique_ptr<general::Url>& Request::url() {
    return impl_->url();
}

Request& Request::url(const general::Url& url) {
    impl_->url(url);
    return *this;
}

Request& Request::url(general::Url&& url) {
    impl_->url(std::move(url));
    return *this;
}

const std::unique_ptr<general::Cookie>& Request::cookie() {
    return impl_->cookie();
}

Request& Request::cookie(const general::Cookie& cookie) {
    impl_->cookie(cookie);
    return *this;
}

Request& Request::cookie(general::Cookie&& cookie) {
    impl_->cookie(std::move(cookie));
    return *this;
}

const std::unique_ptr<general::Header>& Request::header() {
    return impl_->header();
}

Request& Request::header(const general::Header& header) {
    impl_->header(header);
    return *this;
}

Request& Request::header(general::Header&& header) {
    impl_->header(std::move(header));
    return *this;
}

const std::unique_ptr<std::string>& Request::useragent() {
    return impl_->useragent();
}

Request& Request::useragent(const std::string& useragent) {
    impl_->useragent(useragent);
    return *this;
}

Request& Request::useragent(std::string&& useragent) {
    impl_->useragent(std::move(useragent));
    return *this;
}

const std::unique_ptr<general::Authentication>& Request::authentication() {
    return impl_->authentication();
}

Request& Request::authentication(
    const general::Authentication& authentication) {
    impl_->authentication(authentication);
    return *this;
}

Request& Request::authentication(general::Authentication&& authentication) {
    impl_->authentication(std::move(authentication));
    return *this;
}

const std::unique_ptr<std::string>& Request::body() {
    return impl_->body();
}

Request& Request::body(const std::string& body, const std::string& mime_type) {
    impl_->body(body, mime_type);
    return *this;
}

Request& Request::body(std::string&& body, std::string&& mime_type) {
    impl_->body(std::move(body), std::move(mime_type));
    return *this;
}

const std::unique_ptr<Request::MimeParts>& Request::mime_parts() {
    return impl_->mime_parts();
}

Request& Request::mime_parts(const MimeParts& mime_parts) {
    impl_->mime_parts(mime_parts);
    return *this;
}

Request& Request::mime_parts(MimeParts&& mime_parts) {
    impl_->mime_parts(std::move(mime_parts));
    return *this;
}

const std::unique_ptr<Request::Proxy>& Request::proxy() {
    return impl_->proxy();
}

Request& Request::proxy(const Proxy& proxy) {
    impl_->proxy(proxy);
    return *this;
}

Request& Request::proxy(Proxy&& proxy) {
    impl_->proxy(std::move(proxy));
    return *this;
}

const std::unique_ptr<Request::TimeoutSetting>& Request::timeout_setting() {
    return impl_->timeout_setting();
}

Request& Request::timeout_setting(const TimeoutSetting& timeout_setting) {
    impl_->timeout_setting(timeout_setting);
    return *this;
}

Request& Request::timeout_setting(TimeoutSetting&& timeout_setting) {
    impl_->timeout_setting(std::move(timeout_setting));
    return *this;
}

const std::unique_ptr<Request::LocalSetting>& Request::local_setting() {
    return impl_->local_setting();
}

Request& Request::local_setting(const LocalSetting& local_setting) {
    impl_->local_setting(local_setting);
    return *this;
}

Request& Request::local_setting(LocalSetting&& local_setting) {
    impl_->local_setting(std::move(local_setting));
    return *this;
}

bool Request::request() {
    return impl_->request();
}

const std::unique_ptr<Request::Response>& Request::response() {
    return impl_->response();
}

std::time_t Request::parse_time_str(const char* time_str) {
    return curl_getdate(time_str, nullptr);
}

} // namespace network
} // namespace utils
} // namespace rayalto
