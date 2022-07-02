#ifndef RA_UTILS_REQUEST_REQUEST_HPP_
#define RA_UTILS_REQUEST_REQUEST_HPP_

#include <cstdint>
#include <ctime>
#include <initializer_list>
#include <memory>
#include <string>
#include <utility>

#include "rautils/network/general/authentication.h"
#include "rautils/network/general/cookie.h"
#include "rautils/network/general/header.h"
#include "rautils/network/general/url.h"

namespace rayalto::utils::network {

/**
 * Make http request easily, super heavy shitty wrapper for the great curl
 *
 * Note:
 *   - Set mime type in body and mime_parts at the same time may cause strange
 *       problems.
 */
class Request {
public:
    struct TimeoutSetting;
    struct LocalSetting;
    struct Response;
    class MimePart;
    class MimeParts;
    class Proxy;
    enum class Method : std::uint8_t { GET, POST, PUT, DELETE, PATCH, DEFAULT };
    enum class IpResolve : std::uint8_t { WHATEVER, IPv4_ONLY, IPv6_ONLY };

    static constexpr const char* method_c_str(const Method& method);

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
    [[nodiscard]] const Method& method() const;
    Method& method();
    // set request method
    Request& method(Method method);

    // get current setting for ip resolve (whatever/IPv4 only/IPv6 only)
    [[nodiscard]] const IpResolve& ip_resolve() const;
    IpResolve& ip_resolve();
    // select the kind of IP address to use (whatever/IPv4 only/IPv6 only)
    Request& ip_resolve(IpResolve ip_resolve);

    // get current target url
    const std::unique_ptr<general::Url>& url();
    // set target url
    Request& url(const general::Url& url);
    Request& url(general::Url&& url);

    // get cookie to send for current request
    const std::unique_ptr<general::Cookie>& cookie();
    // set cookie to send
    Request& cookie(const general::Cookie& cookie);
    Request& cookie(general::Cookie&& cookie);

    // get header for current request
    const std::unique_ptr<general::Header>& header();
    // set header for current request
    Request& header(const general::Header& header);
    Request& header(general::Header&& header);

    // get current user agent setting
    const std::unique_ptr<std::string>& useragent();
    // set user agent (set user agent header will override this)
    Request& useragent(const std::string& useragent);
    Request& useragent(std::string&& useragent);

    // get current server authentication setting
    const std::unique_ptr<general::Authentication>& authentication();
    // set server authentication (set authentication header will override this)
    Request& authentication(const general::Authentication& authentication);
    Request& authentication(general::Authentication&& authentication);

    // get body for current request
    const std::unique_ptr<std::string>& body();
    // set body for current request, default mime type is
    // "application/x-www-form-urlencoded"
    Request& body(
        const std::string& body,
        const std::string& mime_type = "application/x-www-form-urlencoded");
    Request& body(
        std::string&& body,
        std::string&& mime_type = "application/x-www-form-urlencoded");

    // get multipart/formdata for current request
    const std::unique_ptr<MimeParts>& mime_parts();
    // set multipart/formdata for current request
    Request& mime_parts(const MimeParts& mime_parts);
    Request& mime_parts(MimeParts&& mime_parts);

    // get proxy setting
    const std::unique_ptr<Proxy>& proxy();
    // set proxy
    Request& proxy(const Proxy& proxy);
    Request& proxy(Proxy&& proxy);

    // get timeout setting
    const std::unique_ptr<TimeoutSetting>& timeout_setting();
    // set timeout
    Request& timeout_setting(const TimeoutSetting& timeout_setting);

    const std::unique_ptr<LocalSetting>& local_setting();
    Request& local_setting(const LocalSetting& local_setting);
    Request& local_setting(LocalSetting&& local_setting);

    // apply all settings in one go, then perform the request
    bool request();

    // get the last response
    const std::unique_ptr<Response>& response();

    static std::time_t parse_time_str(const char* time_str);

protected:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

struct Request::TimeoutSetting {
    std::int64_t timeout = 0L;
    std::int64_t connect_timeout = 300000L;
};

struct Request::LocalSetting {
    std::string interface;
    std::int64_t port = 0;
    std::int64_t port_range = 1;
    std::string dns_interface;
    std::string dns_local_ipv4;
    std::string dns_local_ipv6;
};

struct Request::Response {
    struct TimeElapsed {
        // total time elapsed
        double all;
        // time elapsed from start until name resolving completed
        double name_resolve;
        // time elapsed from start until connection(remote_host/proxy) completed
        double connect;
        // time elapsed from start until SSL/SSH handshake completed
        double handshake;
        // time elapsed until file transfer start
        double pre_transfer;
        // time elapsed from start until the first byte is received
        double start_transfer;
    };

    struct Speed {
        std::int64_t download;
        std::int64_t upload;
    };

    struct ByteTransfered {
        std::int64_t download;
        std::int64_t upload;
    };

    struct LocalInfo {
        std::string ip;
        std::int64_t port;
    };

    std::string message;
    std::string body;
    std::int64_t code;
    std::int64_t http_version;
    general::Header header;
    general::Cookie cookie;
    Response::TimeElapsed time_elapsed;
    Response::ByteTransfered byte_transfered;
    Response::Speed speed;
    Response::LocalInfo local_info;
    std::string verbose;
};

class Request::MimePart {
public:
    MimePart() = default;
    MimePart(const MimePart&) = default;
    MimePart(MimePart&&) noexcept = default;
    MimePart& operator=(const MimePart&) = default;
    MimePart& operator=(MimePart&&) noexcept = default;

    virtual ~MimePart() = default;

    // is a file?
    bool& is_file();
    [[nodiscard]] const bool& is_file() const;
    MimePart& is_file(const bool& file);

    // part data or a local file name
    std::string& data();
    [[nodiscard]] const std::string& data() const;
    MimePart& data(const std::string& data);
    MimePart& data(std::string&& data);
    MimePart& data(char* data, std::size_t length);

    // mime type
    std::string& type();
    [[nodiscard]] const std::string& type() const;
    MimePart& type(const std::string& type);
    MimePart& type(std::string&& type);

    // [optional] remote file name, take effect when file() is true
    std::string& file_name();
    [[nodiscard]] const std::string& file_name() const;
    MimePart& file_name(const std::string& file_name);
    MimePart& file_name(std::string&& file_name);

protected:
    // is a file?
    bool is_file_ = false;
    // part data or file name
    std::string data_;
    // mime type
    std::string type_;
    // remote file name
    std::string file_name_;
};

class Request::MimeParts : public misc::Map<MimePart> {
public:
    MimeParts(
        std::initializer_list<std::pair<const std::string, MimePart>> pairs);
    explicit MimeParts(const misc::Map<MimePart>& map);
    explicit MimeParts(misc::Map<MimePart>&& map);
    MimeParts() = default;
    MimeParts(const MimeParts& mime_parts) = default;
    MimeParts(MimeParts&& mime_parts) noexcept = default;
    MimeParts& operator=(const MimeParts& mime_parts) = default;
    MimeParts& operator=(MimeParts&& mime_parts) noexcept = default;

    ~MimeParts() override = default;
};

class Request::Proxy {
public:
    enum class Type : std::uint8_t {
        HTTP,
        HTTPS,
        SOCKS4,
        SOCKS4A,
        SOCKS5,
        SOCKS5H
    };
    static constexpr const char* type_c_str(const Type& type);

    explicit Proxy(const std::string& proxy);
    explicit Proxy(std::string&& proxy);
    Proxy(const Proxy::Type& type,
          const std::string& ip,
          const std::int64_t& port);
    Proxy() = default;
    Proxy(const Proxy&) = default;
    Proxy(Proxy&&) noexcept = default;
    Proxy& operator=(const Proxy&) = default;
    Proxy& operator=(Proxy&&) noexcept = default;

    virtual ~Proxy() = default;

    std::string& str();
    [[nodiscard]] const std::string& str() const;
    void str(const std::string& str);
    void str(std::string&& str);

    // tunneling through http proxy
    bool& http_proxy_tunnel();
    [[nodiscard]] const bool& http_proxy_tunnel() const;
    // whether tunneling through http proxy
    void http_proxy_tunnel(const bool& http_proxy_tunnel);

    void clear() noexcept;

    [[nodiscard]] const char* c_str() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

protected:
    std::string proxy_;
    bool http_proxy_tunnel_ = false;
};

} // namespace rayalto::utils::network

#endif // RA_UTILS_REQUEST_REQUEST_HPP_
