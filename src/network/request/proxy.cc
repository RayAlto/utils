#include "rautils/network/request.h"

#include <string>
#include <utility>

namespace rayalto {
namespace utils {
namespace network {

constexpr const char* Request::Proxy::type_c_str(const Type& type) {
    return type == Type::HTTP      ? "http://"
           : type == Type::HTTPS   ? "https://"
           : type == Type::SOCKS4  ? "socks4://"
           : type == Type::SOCKS4A ? "socks4a://"
           : type == Type::SOCKS5  ? "socks5://"
           : type == Type::SOCKS5H ? "socks5h://"
                                   : nullptr;
}

Request::Proxy::Proxy(const std::string& proxy) : proxy_(proxy) {}

Request::Proxy::Proxy(std::string&& proxy) : proxy_(std::move(proxy)) {}

Request::Proxy::Proxy(const Proxy::Type& type,
                      const std::string& ip,
                      const long& port) :
    proxy_(type_c_str(type) + ip + ':' + std::to_string(port)) {}

std::string& Request::Proxy::str() {
    return proxy_;
}

const std::string& Request::Proxy::str() const {
    return proxy_;
}

void Request::Proxy::str(const std::string& str) {
    proxy_ = str;
}

void Request::Proxy::str(std::string&& str) {
    proxy_ = std::move(str);
}

bool& Request::Proxy::http_proxy_tunnel() {
    return http_proxy_tunnel_;
}

const bool& Request::Proxy::http_proxy_tunnel() const {
    return http_proxy_tunnel_;
}

void Request::Proxy::http_proxy_tunnel(const bool& http_proxy_tunnel) {
    http_proxy_tunnel_ = http_proxy_tunnel;
}

void Request::Proxy::http_proxy_tunnel(bool&& http_proxy_tunnel) {
    http_proxy_tunnel_ = std::move(http_proxy_tunnel);
}

void Request::Proxy::clear() noexcept {
    proxy_.clear();
    http_proxy_tunnel_ = false;
}

const char* Request::Proxy::c_str() const noexcept {
    return proxy_.empty() ? nullptr : proxy_.c_str();
}

bool Request::Proxy::empty() const noexcept {
    return proxy_.empty();
}

} // namespace network
} // namespace utils
} // namespace rayalto
