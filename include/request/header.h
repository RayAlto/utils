#ifndef RA_UTILS_REQUEST_HEADER_HPP_
#define RA_UTILS_REQUEST_HEADER_HPP_

#include <curl/curl.h>
#include <initializer_list>
#include <string>
#include <utility>

#include "util/map_handler.h"

namespace rayalto {
namespace utils {
namespace request {

class Header : public util::DictIcHandler {
public:
    Header(
        std::initializer_list<std::pair<const std::string, std::string>> pairs);
    Header(const util::DictIC& map);
    Header(util::DictIC&& map);
    Header() = default;
    Header(const Header& header);
    Header(Header&& header) noexcept;
    Header& operator=(const Header& header);
    Header& operator=(Header&& header) noexcept;

    virtual ~Header();

    // get curl_slist pointer
    curl_slist* curl_header();

protected:
    curl_slist* curl_header_ = nullptr;
};

} // namespace request
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_HEADER_HPP_
