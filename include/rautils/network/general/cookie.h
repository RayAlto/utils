#ifndef RA_UTILS_RAUTILS_NETWORK_GENERAL_COOKIE_H_
#define RA_UTILS_RAUTILS_NETWORK_GENERAL_COOKIE_H_

#include <initializer_list>
#include <string>
#include <utility>

#include "rautils/misc/map_handler.h"

namespace rayalto {
namespace utils {
namespace network {
namespace general {

class Cookie : public misc::DictHandler {
public:
    Cookie(
        std::initializer_list<std::pair<const std::string, std::string>> pairs);
    Cookie(const misc::Dict& map);
    Cookie(misc::Dict&& map);
    Cookie() = default;
    Cookie(const Cookie&);
    Cookie(Cookie&&) noexcept;
    Cookie& operator=(const Cookie&);
    Cookie& operator=(Cookie&&) noexcept;

    virtual ~Cookie() = default;

    // format as "key1=value1; key2=value2; ..."
    const char* c_str();

protected:
    std::string str_;
};

} // namespace general
} // namespace network
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_NETWORK_GENERAL_H_
