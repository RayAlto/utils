#ifndef RA_UTILS_CRYPTO_RANDOM_H_
#define RA_UTILS_CRYPTO_RANDOM_H_

#include <cstddef>
#include <vector>

namespace rayalto {
namespace utils {
namespace crypto {
namespace random {

std::vector<unsigned char> bytes(const std::size_t& length);

} // namespace random
} // namespace crypto
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_CRYPTO_RANDOM_H_