#include "rautils/network/request.h"

#include <string>
#include <utility>

namespace rayalto {
namespace utils {
namespace network {

Request::Header::Header(
    std::initializer_list<std::pair<const std::string, std::string>> pairs) :
    misc::DictIcHandler(pairs) {}

Request::Header::Header(const misc::DictIC& map) : misc::DictIcHandler(map) {}

Request::Header::Header(misc::DictIC&& map) :
    misc::DictIcHandler(std::move(map)) {}

Request::Header::Header(const Header& header) :
    misc::DictIcHandler(header.map_) {}

Request::Header::Header(Header&& header) noexcept :
    misc::DictIcHandler(std::move(header.map_)) {}

Request::Header& Request::Header::operator=(const Header& header) {
    map_ = header.map_;
    return *this;
}

Request::Header& Request::Header::operator=(Header&& header) noexcept {
    map_ = std::move(header.map_);
    return *this;
}

} // namespace network
} // namespace utils
} // namespace rayalto
