#ifndef RA_UTILS_REQUEST_METHOD_HPP_
#define RA_UTILS_REQUEST_METHOD_HPP_

#include <cstdint>
#include <string>

namespace rayalto {
namespace utils {
namespace request {

enum class Method : std::uint8_t { GET, POST, PUT, DELETE, PATCH };

namespace method {

constexpr const char* c_str(const Method& method) {
    return method == Method::GET      ? "GET"
           : method == Method::POST   ? "POST"
           : method == Method::PUT    ? "PUT"
           : method == Method::DELETE ? "DELETE"
           : method == Method::PATCH  ? "PATCH"
                                      : nullptr;
}

} // namespace method
} // namespace request
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_METHOD_HPP_
