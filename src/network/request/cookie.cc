#include "rautils/network/request.h"

#include <string>
#include <utility>
#include <vector>

#include "curl/curl.h"

#include "rautils/string/strtool.h"
#include "rautils/misc/map_handler.h"

namespace rayalto {
namespace utils {
namespace network {

Request::Cookie::Cookie(
    std::initializer_list<std::pair<const std::string, std::string>> pairs) :
    misc::DictHandler(pairs) {}

Request::Cookie::Cookie(const misc::Dict& map) : misc::DictHandler(map) {}

Request::Cookie::Cookie(misc::Dict&& map) : misc::DictHandler(std::move(map)) {}

Request::Cookie::Cookie(const Cookie& cookie) :
    misc::DictHandler(cookie.map_) {}

Request::Cookie::Cookie(Cookie&& cookie) noexcept :
    misc::DictHandler(std::move(cookie.map_)) {}

Request::Cookie& Request::Cookie::operator=(const Cookie& cookie) {
    this->map_ = cookie.map_;
    return *this;
}

Request::Cookie& Request::Cookie::operator=(Cookie&& cookie) noexcept {
    this->map_ = std::move(cookie.map_);
    return *this;
}

const char* Request::Cookie::c_str() {
    str_ = string::kv_format(map_, '=', false, ';');
    return str_.c_str();
}

} // namespace network
} // namespace utils
} // namespace rayalto
