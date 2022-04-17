#ifndef RA_UTILS_REQUEST_COOKIE_HPP_
#define RA_UTILS_REQUEST_COOKIE_HPP_

#include <initializer_list>
#include <string>
#include <utility>

#include "util/map_handler.h"

namespace rayalto {
namespace utils {
namespace request {

class Cookie : public util::DictHandler {
public:
    Cookie(
        std::initializer_list<std::pair<const std::string, std::string>> pairs);
    Cookie(const util::Dict& map);
    Cookie(util::Dict&& map);
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

} // namespace request
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_COOKIE_HPP_
