#include "crypto/random.h"

#include <cstddef>
#include <string>

#include "openssl/rand.h"

namespace rayalto {
namespace utils {
namespace crypto {
namespace random {

std::string bytes(const std::size_t& length) {
    unsigned char buffer[length + 1];
    if (RAND_bytes(buffer, length) <= 0) {
        return std::string {};
    }
    return std::string(reinterpret_cast<char*>(buffer), length);
}

} // namespace random
} // namespace crypto
} // namespace utils
} // namespace rayalto
