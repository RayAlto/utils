#ifndef RA_UTILS_RAUTILS_NETWORK_GENERAL_HEADER_H_
#define RA_UTILS_RAUTILS_NETWORK_GENERAL_HEADER_H_

#include <initializer_list>
#include <string>
#include <utility>

#include "rautils/misc/map_handler.h"

namespace rayalto {
namespace utils {
namespace network {
namespace general {

class Header : public misc::DictIcHandler {
public:
    Header(
        std::initializer_list<std::pair<const std::string, std::string>> pairs);
    Header(const misc::DictIC& map);
    Header(misc::DictIC&& map);
    Header() = default;
    Header(const Header& header);
    Header(Header&& header) noexcept;
    Header& operator=(const Header& header);
    Header& operator=(Header&& header) noexcept;

    virtual ~Header() = default;
};

} // namespace general
} // namespace network
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_NETWORK_GENERAL_H_
