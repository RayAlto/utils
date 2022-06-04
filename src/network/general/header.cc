#include "rautils/network/request.h"

#include <string>
#include <utility>

namespace rayalto {
namespace utils {
namespace network {
namespace general {

Header::Header(
    std::initializer_list<std::pair<const std::string, std::string>> pairs) :
    misc::DictIcHandler(pairs) {}

Header::Header(const misc::DictIC& map) : misc::DictIcHandler(map) {}

Header::Header(misc::DictIC&& map) : misc::DictIcHandler(std::move(map)) {}

Header::Header(const Header& header) : misc::DictIcHandler(header.map_) {}

Header::Header(Header&& header) noexcept :
    misc::DictIcHandler(std::move(header.map_)) {}

Header& Header::operator=(const Header& header) {
    map_ = header.map_;
    return *this;
}

Header& Header::operator=(Header&& header) noexcept {
    map_ = std::move(header.map_);
    return *this;
}

} // namespace general
} // namespace network
} // namespace utils
} // namespace rayalto
