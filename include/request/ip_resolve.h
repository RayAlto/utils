#ifndef RA_UTILS_REQUEST_IP_RESOLVE_H_
#define RA_UTILS_REQUEST_IP_RESOLVE_H_

#include <cstdint>

#include <curl/curl.h>

namespace rayalto {
namespace utils {
namespace request {

enum class IP_Resolve : std::uint8_t { WHATEVER, IPv4_ONLY, IPv6_ONLY };

namespace ip_resolve {

constexpr long as_option(const request::IP_Resolve& ip_resolve) {
    return ip_resolve == request::IP_Resolve::IPv4_ONLY ? CURL_IPRESOLVE_V4
           : ip_resolve == request::IP_Resolve::IPv6_ONLY
               ? CURL_IPRESOLVE_V6
               : CURL_IPRESOLVE_WHATEVER;
}

} // namespace ip_resolve

} // namespace request
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_IP_RESOLVE_H_
