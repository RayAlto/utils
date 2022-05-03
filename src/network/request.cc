#include "rautils/network/request.h"

#include <cstddef>
#include <cstdio>
#include <cstring> // std::strlen, std::size_t
#include <ctime>
#include <memory>
#include <string>
#include <utility>

#include "curl/curl.h"

#include "rautils/string/strtool.h"

namespace rayalto {
namespace utils {
namespace network {

namespace {

void parse_header(const char* data,
                  const std::size_t& size,
                  Request::Header& result) {
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

void init_curl_header(const Request::Header& headers,
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

void parse_cookie_slist(curl_slist* curl_cookies, Request::Cookie& cookie) {
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
                                        Request::Header* userdata) {
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

class Request::Impl {
public:
    Impl();
    Impl(const Impl&) = delete;
    Impl(Impl&&) noexcept = default;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&) noexcept = default;

    virtual ~Impl();

    static std::string curl_version();
    void reset();

    Method method();
    Impl& method(Method method);

    IpResolve ip_resolve();
    Impl& ip_resolve(IpResolve ip_resolve);

    std::string url();
    Impl& url(const std::string& url);
    Impl& url(std::string&& url);

    Cookie cookie();
    Impl& cookie(const Cookie& cookie);
    Impl& cookie(Cookie&& cookie);

    Header header();
    Impl& header(const Header& header);
    Impl& header(Header&& header);

    std::string useragent();
    Impl& useragent(const std::string& useragent);
    Impl& useragent(std::string&& useragent);

    Authentication authentication();
    Impl& authentication(const Authentication& authentication);
    Impl& authentication(Authentication&& authentication);

    std::string body();
    Impl& body(
        const std::string& body,
        const std::string& mime_type = "application/x-www-form-urlencoded");
    Impl& body(std::string&& body,
               std::string&& mime_type = "application/x-www-form-urlencoded");

    MimeParts mime_parts();
    Impl& mime_parts(const MimeParts& mime_parts);
    Impl& mime_parts(MimeParts&& mime_parts);

    Proxy proxy();
    Impl& proxy(const Proxy& proxy);
    Impl& proxy(Proxy&& proxy);

    long timeout();
    Impl& timeout(const long& timeout);
    Impl& timeout(long&& timeout);

    long connect_timeout();
    Impl& connect_timeout(const long& connect_timeout);
    Impl& connect_timeout(long&& connect_timeout);

    std::string local_interface();
    Impl& local_interface(const std::string& local_interface);
    Impl& local_interface(std::string&& local_interface);

    long local_port();
    Impl& local_port(const long& local_port);
    Impl& local_port(long&& local_port);

    long local_port_range();
    Impl& local_port_range(const long& local_port_range);
    Impl& local_port_range(long&& local_port_range);

    std::string dns_interface();
    Impl& dns_interface(const std::string& dns_interface);
    Impl& dns_interface(std::string&& dns_interface);

    std::string dns_local_ipv4();
    Impl& dns_local_ipv4(const std::string& dns_local_ipv4);
    Impl& dns_local_ipv4(std::string&& dns_local_ipv4);

    std::string dns_local_ipv6();
    Impl& dns_local_ipv6(const std::string& dns_local_ipv6);
    Impl& dns_local_ipv6(std::string&& dns_local_ipv6);

    bool request();

    Response response();

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
    std::unique_ptr<std::string> url_ = nullptr;
    std::unique_ptr<Cookie> cookie_ = nullptr;
    std::unique_ptr<Header> header_ = nullptr;
    std::unique_ptr<std::string> useragent_ = nullptr;
    std::unique_ptr<Authentication> authentication_ = nullptr;
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

std::string Request::Impl::curl_version() {
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

Request::Method Request::Impl::method() {
    return method_;
}

Request::Impl& Request::Impl::method(Method method) {
    method_ = method;
    return *this;
}

Request::IpResolve Request::Impl::ip_resolve() {
    return ip_resolve_;
}

Request::Impl& Request::Impl::ip_resolve(IpResolve ip_resolve) {
    ip_resolve_ = ip_resolve;
    return *this;
}

std::string Request::Impl::url() {
    if (url_ == nullptr) {
        url_ = std::make_unique<std::string>();
    }
    return *url_;
}

Request::Impl& Request::Impl::url(const std::string& url) {
    url_ = std::make_unique<std::string>(url);
    return *this;
}

Request::Impl& Request::Impl::url(std::string&& url) {
    url_ = std::make_unique<std::string>(std::move(url));
    return *this;
}

Request::Cookie Request::Impl::cookie() {
    if (cookie_ == nullptr) {
        cookie_ = std::make_unique<Cookie>();
    }
    return *cookie_;
}

Request::Impl& Request::Impl::cookie(const Cookie& cookie) {
    cookie_ = std::make_unique<Cookie>(cookie);
    return *this;
}

Request::Impl& Request::Impl::cookie(Cookie&& cookie) {
    cookie_ = std::make_unique<Cookie>(std::move(cookie));
    return *this;
}

Request::Header Request::Impl::header() {
    if (header_ == nullptr) {
        header_ = std::make_unique<Header>();
    }
    return *header_;
}

Request::Impl& Request::Impl::header(const Header& header) {
    header_ = std::make_unique<Header>(header);
    return *this;
}

Request::Impl& Request::Impl::header(Header&& header) {
    header_ = std::make_unique<Header>(std::move(header));
    return *this;
}

std::string Request::Impl::useragent() {
    if (useragent_ == nullptr) {
        useragent_ = std::make_unique<std::string>();
    }
    return *useragent_;
}

Request::Impl& Request::Impl::useragent(const std::string& useragent) {
    useragent_ = std::make_unique<std::string>(useragent);
    return *this;
}

Request::Impl& Request::Impl::useragent(std::string&& useragent) {
    useragent_ = std::make_unique<std::string>(std::move(useragent));
    return *this;
}

Request::Authentication Request::Impl::authentication() {
    if (authentication_ == nullptr) {
        authentication_ = std::make_unique<Authentication>();
    }
    return *authentication_;
}

Request::Impl& Request::Impl::authentication(
    const Authentication& authentication) {
    authentication_ = std::make_unique<Authentication>(authentication);
    return *this;
}

Request::Impl& Request::Impl::authentication(Authentication&& authentication) {
    authentication_ =
        std::make_unique<Authentication>(std::move(authentication));
    return *this;
}

std::string Request::Impl::body() {
    if (body_ == nullptr) {
        body_ = std::make_unique<std::string>();
    }
    return *body_;
}

Request::Impl& Request::Impl::body(const std::string& body,
                                   const std::string& mime_type) {
    body_ = std::make_unique<std::string>(body);
    if (header_ == nullptr) {
        header_ = std::make_unique<Header>();
    }
    (*header_)["content-type"] = mime_type;
    return *this;
}

Request::Impl& Request::Impl::body(std::string&& body,
                                   std::string&& mime_type) {
    body_ = std::make_unique<std::string>(std::move(body));
    if (header_ == nullptr) {
        header_ = std::make_unique<Header>();
    }
    (*header_)["content-type"] = std::move(mime_type);
    return *this;
}

Request::MimeParts Request::Impl::mime_parts() {
    if (mime_parts_ == nullptr) {
        mime_parts_ = std::make_unique<MimeParts>();
    }
    return *mime_parts_;
}

Request::Impl& Request::Impl::mime_parts(const MimeParts& mime_parts) {
    mime_parts_ = std::make_unique<MimeParts>(mime_parts);
    return *this;
}

Request::Impl& Request::Impl::mime_parts(MimeParts&& mime_parts) {
    mime_parts_ = std::make_unique<MimeParts>(std::move(mime_parts));
    return *this;
}

Request::Proxy Request::Impl::proxy() {
    if (proxy_ == nullptr) {
        proxy_ = std::make_unique<Proxy>();
    }
    return *proxy_;
}

Request::Impl& Request::Impl::proxy(const Proxy& proxy) {
    proxy_ = std::make_unique<Proxy>(proxy);
    return *this;
}

Request::Impl& Request::Impl::proxy(Proxy&& proxy) {
    proxy_ = std::make_unique<Proxy>(std::move(proxy));
    return *this;
}

long Request::Impl::timeout() {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    return timeout_setting_->timeout;
}

Request::Impl& Request::Impl::timeout(const long& timeout) {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    timeout_setting_->timeout = timeout;
    return *this;
}

Request::Impl& Request::Impl::timeout(long&& timeout) {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    timeout_setting_->timeout = std::move(timeout);
    return *this;
}

long Request::Impl::connect_timeout() {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    return timeout_setting_->connect_timeout;
}

Request::Impl& Request::Impl::connect_timeout(const long& connect_timeout) {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    timeout_setting_->connect_timeout = connect_timeout;
    return *this;
}

Request::Impl& Request::Impl::connect_timeout(long&& connect_timeout) {
    if (timeout_setting_ == nullptr) {
        timeout_setting_ = std::make_unique<TimeoutSetting>();
    }
    timeout_setting_->connect_timeout = std::move(connect_timeout);
    return *this;
}

std::string Request::Impl::local_interface() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->interface;
}

Request::Impl& Request::Impl::local_interface(
    const std::string& local_interface) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->interface = local_interface;
    return *this;
}

Request::Impl& Request::Impl::local_interface(std::string&& local_interface) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->interface = std::move(local_interface);
    return *this;
}

long Request::Impl::local_port() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->port;
}

Request::Impl& Request::Impl::local_port(const long& local_port) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->port = local_port;
    return *this;
}

Request::Impl& Request::Impl::local_port(long&& local_port) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->port = std::move(local_port);
    return *this;
}

long Request::Impl::local_port_range() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->port_range;
}

Request::Impl& Request::Impl::local_port_range(const long& local_port_range) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->port_range = local_port_range;
    return *this;
}

Request::Impl& Request::Impl::local_port_range(long&& local_port_range) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->port_range = std::move(local_port_range);
    return *this;
}

std::string Request::Impl::dns_interface() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->dns_interface;
}

Request::Impl& Request::Impl::dns_interface(const std::string& dns_interface) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_interface = dns_interface;
    return *this;
}

Request::Impl& Request::Impl::dns_interface(std::string&& dns_interface) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_interface = std::move(dns_interface);
    return *this;
}

std::string Request::Impl::dns_local_ipv4() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->dns_local_ipv4;
}

Request::Impl& Request::Impl::dns_local_ipv4(
    const std::string& dns_local_ipv4) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_local_ipv4 = dns_local_ipv4;
    return *this;
}

Request::Impl& Request::Impl::dns_local_ipv4(std::string&& dns_local_ipv4) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_local_ipv4 = std::move(dns_local_ipv4);
    return *this;
}

std::string Request::Impl::dns_local_ipv6() {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    return local_setting_->dns_local_ipv6;
}

Request::Impl& Request::Impl::dns_local_ipv6(
    const std::string& dns_local_ipv6) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_local_ipv6 = dns_local_ipv6;
    return *this;
}

Request::Impl& Request::Impl::dns_local_ipv6(std::string&& dns_local_ipv6) {
    if (local_setting_ == nullptr) {
        local_setting_ = std::make_unique<LocalSetting>();
    }
    local_setting_->dns_local_ipv6 = std::move(dns_local_ipv6);
    return *this;
}

bool Request::Impl::request() {
    init_curl_handle_();
    set_options_();
    return perform_request_();
}

Request::Response Request::Impl::response() {
    if (response_ == nullptr) {
        response_ = std::make_unique<Response>();
    }
    return *response_;
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
    init_curl_header(*header_, &curl_header_);
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

Request::Method Request::method() {
    return impl_->method();
}

Request& Request::method(Method method) {
    impl_->method(method);
    return *this;
}

Request::IpResolve Request::ip_resolve() {
    return impl_->ip_resolve();
}

Request& Request::ip_resolve(IpResolve ip_resolve) {
    impl_->ip_resolve(ip_resolve);
    return *this;
}

std::string Request::url() {
    return impl_->url();
}

Request& Request::url(const std::string& url) {
    impl_->url(url);
    return *this;
}

Request& Request::url(std::string&& url) {
    impl_->url(std::move(url));
    return *this;
}

Request::Cookie Request::cookie() {
    return impl_->cookie();
}

Request& Request::cookie(const Cookie& cookie) {
    impl_->cookie(cookie);
    return *this;
}

Request& Request::cookie(Cookie&& cookie) {
    impl_->cookie(std::move(cookie));
    return *this;
}

Request::Header Request::header() {
    return impl_->header();
}

Request& Request::header(const Header& header) {
    impl_->header(header);
    return *this;
}

Request& Request::header(Header&& header) {
    impl_->header(std::move(header));
    return *this;
}

std::string Request::useragent() {
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

Request::Authentication Request::authentication() {
    return impl_->authentication();
}

Request& Request::authentication(const Authentication& authentication) {
    impl_->authentication(authentication);
    return *this;
}

Request& Request::authentication(Authentication&& authentication) {
    impl_->authentication(std::move(authentication));
    return *this;
}

std::string Request::body() {
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

Request::MimeParts Request::mime_parts() {
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

Request::Proxy Request::proxy() {
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

long Request::timeout() {
    return impl_->timeout();
}

Request& Request::timeout(const long& timeout) {
    impl_->timeout(timeout);
    return *this;
}

Request& Request::timeout(long&& timeout) {
    impl_->timeout(std::move(timeout));
    return *this;
}

long Request::connect_timeout() {
    return impl_->connect_timeout();
}

Request& Request::connect_timeout(const long& connect_timeout) {
    impl_->connect_timeout(connect_timeout);
    return *this;
}

Request& Request::connect_timeout(long&& connect_timeout) {
    impl_->connect_timeout(std::move(connect_timeout));
    return *this;
}

std::string Request::local_interface() {
    return impl_->local_interface();
}

Request& Request::local_interface(const std::string& local_interface) {
    impl_->local_interface(local_interface);
    return *this;
}

Request& Request::local_interface(std::string&& local_interface) {
    impl_->local_interface(std::move(local_interface));
    return *this;
}

long Request::local_port() {
    return impl_->local_port();
}

Request& Request::local_port(const long& local_port) {
    impl_->local_port(local_port);
    return *this;
}

Request& Request::local_port(long&& local_port) {
    impl_->local_port(std::move(local_port));
    return *this;
}

long Request::local_port_range() {
    return impl_->local_port_range();
}

Request& Request::local_port_range(const long& local_port_range) {
    impl_->local_port_range(local_port_range);
    return *this;
}

Request& Request::local_port_range(long&& local_port_range) {
    impl_->local_port_range(std::move(local_port_range));
    return *this;
}

std::string Request::dns_interface() {
    return impl_->dns_interface();
}

Request& Request::dns_interface(const std::string& dns_interface) {
    impl_->dns_interface(dns_interface);
    return *this;
}

Request& Request::dns_interface(std::string&& dns_interface) {
    impl_->dns_interface(std::move(dns_interface));
    return *this;
}

std::string Request::dns_local_ipv4() {
    return impl_->dns_local_ipv4();
}

Request& Request::dns_local_ipv4(const std::string& dns_local_ipv4) {
    impl_->dns_local_ipv4(dns_local_ipv4);
    return *this;
}

Request& Request::dns_local_ipv4(std::string&& dns_local_ipv4) {
    impl_->dns_local_ipv4(std::move(dns_local_ipv4));
    return *this;
}

std::string Request::dns_local_ipv6() {
    return impl_->dns_local_ipv6();
}

Request& Request::dns_local_ipv6(const std::string& dns_local_ipv6) {
    impl_->dns_local_ipv6(dns_local_ipv6);
    return *this;
}

Request& Request::dns_local_ipv6(std::string&& dns_local_ipv6) {
    impl_->dns_local_ipv6(std::move(dns_local_ipv6));
    return *this;
}

bool Request::request() {
    return impl_->request();
}

Request::Response Request::response() {
    return impl_->response();
}

std::time_t Request::parse_time_str(const char* time_str) {
    return curl_getdate(time_str, nullptr);
}

} // namespace network
} // namespace utils
} // namespace rayalto
