#ifndef RA_UTILS_CRYPTO_RANDOM_H_
#define RA_UTILS_CRYPTO_RANDOM_H_

#include <cstddef>
#include <vector>

namespace rayalto::utils::crypto::random {

std::vector<unsigned char> bytes(const int& length);

} // namespace rayalto::utils::crypto::random

#endif // RA_UTILS_CRYPTO_RANDOM_H_
