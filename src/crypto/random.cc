#include "rautils/crypto/random.h"

#include <cstddef>
#include <vector>

#include "openssl/rand.h"

namespace rayalto::utils::crypto::random {

std::vector<unsigned char> bytes(const int& length) {
    unsigned char buffer[length + 1];
    if (RAND_bytes(buffer, length) <= 0) {
        return std::vector<unsigned char> {};
    }
    return {buffer, buffer + length};
}

} // namespace rayalto::utils::crypto::random
