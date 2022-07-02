#ifndef RA_UTILS_CRYPTO_HASH_H_
#define RA_UTILS_CRYPTO_HASH_H_

#include <cstddef>
#include <vector>

namespace rayalto::utils::crypto::hash {

// sha-256 digest, empty when openssl failed
std::vector<unsigned char> sha256(const std::vector<unsigned char>& message);
std::vector<unsigned char> sha256(const std::vector<unsigned char>& message,
                                  const std::size_t& message_length);
std::vector<unsigned char> sha256(const char* message);
std::vector<unsigned char> sha256(const char* message,
                                  const std::size_t& message_length);
std::vector<unsigned char> sha256(const unsigned char* message);
std::vector<unsigned char> sha256(const unsigned char* message,
                                  const std::size_t& message_length);

} // namespace rayalto::utils::crypto::hash

#endif // RA_UTILS_CRYPTO_HASH_H_
