#include "request/proxy.h"

#include <string>
#include <utility>

namespace rayalto {
namespace utils {
namespace request {

Proxy::Proxy(const std::string& proxy) : proxy_(proxy) {}

Proxy::Proxy(std::string&& proxy) : proxy_(std::move(proxy)) {}

Proxy::Proxy(const proxy::Type& type, const std::string& ip, const long& port) :
    proxy_(proxy::type::c_str(type) + ip + ':' + std::to_string(port)) {}

std::string& Proxy::str() {
    return proxy_;
}

const std::string& Proxy::str() const {
    return proxy_;
}

void Proxy::str(const std::string& str) {
    proxy_ = str;
}

void Proxy::str(std::string&& str) {
    proxy_ = std::move(str);
}

bool& Proxy::http_proxy_tunnel() {
    return http_proxy_tunnel_;
}

const bool& Proxy::http_proxy_tunnel() const {
    return http_proxy_tunnel_;
}

void Proxy::http_proxy_tunnel(const bool& http_proxy_tunnel) {
    http_proxy_tunnel_ = http_proxy_tunnel;
}

void Proxy::http_proxy_tunnel(bool&& http_proxy_tunnel) {
    http_proxy_tunnel_ = std::move(http_proxy_tunnel);
}

void Proxy::clear() noexcept {
    proxy_.clear();
    http_proxy_tunnel_ = false;
}

const char* Proxy::c_str() const noexcept {
    return proxy_.empty() ? nullptr : proxy_.c_str();
}

bool Proxy::empty() const noexcept {
    return proxy_.empty();
}

} // namespace request
} // namespace utils
} // namespace rayalto
