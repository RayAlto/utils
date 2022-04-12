#ifndef RA_UTILS_CRYPTO_HASH_H_
#define RA_UTILS_CRYPTO_HASH_H_

#include <array>
#include <cstddef>
#include <string>

namespace rayalto {
namespace utils {
namespace crypto {
namespace hash {

// sha-256 digest, empty when openssl failed
std::string sha256(const std::string& message);
std::string sha256(const std::string& message,
                   const std::size_t& message_length);
std::string sha256(const char* message);
std::string sha256(const char* message, const std::size_t& message_length);
std::string sha256(const unsigned char* message);
std::string sha256(const unsigned char* message,
                   const std::size_t& message_length);

} // namespace hash
} // namespace crypto
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_CRYPTO_HASH_H_
