#ifndef RA_UTILS_REQUEST_PROXY_H_
#define RA_UTILS_REQUEST_PROXY_H_

#include <cstdint>
#include <string>

namespace rayalto {
namespace utils {
namespace request {

namespace proxy {

enum class Type : std::uint8_t {
    HTTP,
    HTTPS,
    SOCKS4,
    SOCKS4A,
    SOCKS5,
    SOCKS5H
};

namespace type {

constexpr const char* c_str(const Type& type) {
    return type == Type::HTTP      ? "http://"
           : type == Type::HTTPS   ? "https://"
           : type == Type::SOCKS4  ? "socks4://"
           : type == Type::SOCKS4A ? "socks4a://"
           : type == Type::SOCKS5  ? "socks5://"
           : type == Type::SOCKS5H ? "socks5h://"
                                   : nullptr;
}

} // namespace type

} // namespace proxy

class Proxy {
public:
    Proxy(const std::string& proxy);
    Proxy(std::string&& proxy);
    Proxy(const proxy::Type& type, const std::string& ip, const long& port);
    Proxy() = default;
    Proxy(const Proxy&) = default;
    Proxy(Proxy&&) noexcept = default;
    Proxy& operator=(const Proxy&) = default;
    Proxy& operator=(Proxy&&) noexcept = default;

    virtual ~Proxy() = default;

    std::string& str();
    const std::string& str() const;
    void str(const std::string& str);
    void str(std::string&& str);

    // tunneling through http proxy
    bool& http_proxy_tunnel();
    const bool& http_proxy_tunnel() const;
    // whether tunneling through http proxy
    void http_proxy_tunnel(const bool& http_proxy_tunnel);
    void http_proxy_tunnel(bool&& http_proxy_tunnel);

    const char* c_str() const noexcept;
    bool empty() const noexcept;

protected:
    std::string proxy_;
    bool http_proxy_tunnel_ = false;
};

} // namespace request
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_PROXY_H_
