#include "request/cookie.h"

#include <string>
#include <utility>
#include <vector>

#include <curl/curl.h>

#include "string/strtool.h"
#include "util/map_handler.h"

namespace rayalto {
namespace utils {
namespace request {

Cookie::Cookie(
    std::initializer_list<std::pair<const std::string, std::string>> pairs) :
    util::DictHandler(pairs) {}

Cookie::Cookie(const util::Dict& map) : util::DictHandler(map) {}

Cookie::Cookie(util::Dict&& map) : util::DictHandler(std::move(map)) {}

Cookie::Cookie(const Cookie& cookie) : util::DictHandler(cookie.map_) {}

Cookie::Cookie(Cookie&& cookie) noexcept :
    util::DictHandler(std::move(cookie.map_)) {}

Cookie& Cookie::operator=(const Cookie& cookie) {
    this->map_ = cookie.map_;
    return *this;
}

Cookie& Cookie::operator=(Cookie&& cookie) noexcept {
    this->map_ = std::move(cookie.map_);
    return *this;
}

void Cookie::from_curl_slist(curl_slist* curl_cookies) {
    if (!curl_cookies) {
        return;
    }
    for (curl_slist* cookie = curl_cookies; cookie; cookie = cookie->next) {
        std::vector<std::string> parts = string::split(cookie->data, '\t');
        map_[parts[5]] = parts[6];
    }
}

const char* Cookie::c_str() {
    str_ = string::kv_format(map_, '=', false, ';');
    return str_.c_str();
}

} // namespace request
} // namespace utils
} // namespace rayalto
