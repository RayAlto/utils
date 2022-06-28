#include "rautils/network/general/cookie.h"

#include <string>
#include <utility>
#include <vector>

#include "curl/curl.h"

#include "rautils/string/strtool.h"
#include "rautils/misc/map_handler.h"

namespace rayalto {
namespace utils {
namespace network {
namespace general {

Cookie::Cookie(
    std::initializer_list<std::pair<const std::string, std::string>> pairs) :
    misc::DictHandler(pairs) {}

Cookie::Cookie(const misc::Dict& map) : misc::DictHandler(map) {}

Cookie::Cookie(misc::Dict&& map) : misc::DictHandler(std::move(map)) {}

Cookie::Cookie(const Cookie& cookie) : misc::DictHandler(cookie.map_) {}

Cookie::Cookie(Cookie&& cookie) noexcept :
    misc::DictHandler(std::move(cookie.map_)) {}

Cookie& Cookie::operator=(const Cookie& cookie) {
    if (this == &cookie) {
        return *this;
    }
    this->map_ = cookie.map_;
    return *this;
}

Cookie& Cookie::operator=(Cookie&& cookie) noexcept {
    if (this == &cookie) {
        return *this;
    }
    this->map_ = std::move(cookie.map_);
    return *this;
}

const char* Cookie::c_str() {
    str_ = string::kv_format(map_, '=', false, ';');
    return str_.c_str();
}

} // namespace general
} // namespace network
} // namespace utils
} // namespace rayalto
