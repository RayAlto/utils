#include "crypto/random.h"

#include <cstddef>
#include <vector>

#include "openssl/rand.h"

namespace rayalto {
namespace utils {
namespace crypto {
namespace random {

std::vector<unsigned char> bytes(const std::size_t& length) {
    unsigned char buffer[length + 1];
    if (RAND_bytes(buffer, length) <= 0) {
        return std::vector<unsigned char> {};
    }
    return std::vector<unsigned char>(buffer, buffer + length);
}

} // namespace random
} // namespace crypto
} // namespace utils
} // namespace rayalto
