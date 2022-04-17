#include "request/header.h"

#include <string>
#include <utility>

#include <curl/curl.h>

namespace rayalto {
namespace utils {
namespace request {

Header::Header(
    std::initializer_list<std::pair<const std::string, std::string>> pairs) :
    util::DictIcHandler(pairs) {}

Header::Header(const util::DictIC& map) : util::DictIcHandler(map) {}

Header::Header(util::DictIC&& map) : util::DictIcHandler(std::move(map)) {}

Header::Header(const Header& header) : util::DictIcHandler(header.map_) {}

Header::Header(Header&& header) noexcept :
    util::DictIcHandler(std::move(header.map_)) {}

Header& Header::operator=(const Header& header) {
    map_ = header.map_;
    return *this;
}

Header& Header::operator=(Header&& header) noexcept {
    map_ = std::move(header.map_);
    return *this;
}

} // namespace request
} // namespace utils
} // namespace rayalto
