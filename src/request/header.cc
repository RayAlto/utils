#include "request/header.h"

#include <string>
#include <utility>

#include <curl/curl.h>

namespace rayalto {
namespace utils {
namespace request {

Header::Header(
    std::initializer_list<std::pair<const std::string, std::string>> pairs) :
    util::MapIcHandler(pairs) {}

Header::Header(const util::MapIc& map) : util::MapIcHandler(map) {}

Header::Header(util::MapIc&& map) : util::MapIcHandler(std::move(map)) {}

Header::Header(const Header& header) : util::MapIcHandler(header.map_) {}

Header::Header(Header&& header) noexcept :
    util::MapIcHandler(std::move(header.map_)) {}

Header& Header::operator=(const Header& header) {
    map_ = header.map_;
    return *this;
}

Header& Header::operator=(Header&& header) noexcept {
    map_ = std::move(header.map_);
    return *this;
}

Header::~Header() {
    if (curl_header_) {
        curl_slist_free_all(curl_header_);
        curl_header_ = nullptr;
    }
}

curl_slist* Header::curl_header() {
    if (curl_header_) {
        curl_slist_free_all(curl_header_);
        curl_header_ = nullptr;
    }
    std::string header_line;
    for (const std::pair<std::string, std::string>& header : map_) {
        header_line = header.first + ": " + header.second;
        curl_header_ = curl_slist_append(curl_header_, header_line.c_str());
    }
    return curl_header_;
}

} // namespace request
} // namespace utils
} // namespace rayalto
