#ifndef RA_UTILS_RAUTILS_NETWORK_GENERAL_COOKIE_H_
#define RA_UTILS_RAUTILS_NETWORK_GENERAL_COOKIE_H_

#include <initializer_list>
#include <string>
#include <utility>

#include "rautils/misc/map_handler.h"

namespace rayalto::utils::network::general {

class Cookie : public misc::DictHandler {
public:
    Cookie(
        std::initializer_list<std::pair<const std::string, std::string>> pairs);
    explicit Cookie(const misc::Dict& map);
    explicit Cookie(misc::Dict&& map);
    Cookie() = default;
    Cookie(const Cookie&);
    Cookie(Cookie&&) noexcept;
    Cookie& operator=(const Cookie&);
    Cookie& operator=(Cookie&&) noexcept;

    ~Cookie() override = default;

    // format as "key1=value1; key2=value2; ..."
    const char* c_str();

protected:
    std::string str_;
};

} // namespace rayalto::utils::network::general

#endif // RA_UTILS_RAUTILS_NETWORK_GENERAL_H_
