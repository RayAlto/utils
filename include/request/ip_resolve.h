#ifndef RA_UTILS_REQUEST_IP_RESOLVE_H_
#define RA_UTILS_REQUEST_IP_RESOLVE_H_

#include <cstdint>

namespace rayalto {
namespace utils {
namespace request {

enum class IP_Resolve : std::uint8_t { WHATEVER, IPv4_ONLY, IPv6_ONLY };

} // namespace request
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_IP_RESOLVE_H_
