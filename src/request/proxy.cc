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

const char* Proxy::c_str() const noexcept {
    return proxy_.empty() ? nullptr : proxy_.c_str();
}

bool Proxy::empty() const noexcept {
    return proxy_.empty();
}

} // namespace request
} // namespace utils
} // namespace rayalto
