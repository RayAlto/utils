#include "crypto/hash.h"

#include <array>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>

#include "openssl/evp.h"
#include "openssl/ossl_typ.h"

namespace rayalto {
namespace utils {
namespace crypto {
namespace hash {

std::array<unsigned char, 32> sha256(const std::string& message) {
    return sha256(reinterpret_cast<const unsigned char*>(message.data()),
                  message.length());
}

std::array<unsigned char, 32> sha256(const std::string& message,
                                     const std::size_t& message_length) {
    return sha256(reinterpret_cast<const unsigned char*>(message.data()),
                  message_length);
}

std::array<unsigned char, 32> sha256(const char* message) {
    return sha256(reinterpret_cast<const unsigned char*>(message),
                  std::strlen(message));
}

std::array<unsigned char, 32> sha256(const char* message,
                                     const std::size_t& message_length) {
    return sha256(reinterpret_cast<const unsigned char*>(message),
                  message_length);
}

std::array<unsigned char, 32> sha256(const unsigned char* message) {
    return sha256(message, std::strlen(reinterpret_cast<const char*>(message)));
}

std::array<unsigned char, 32> sha256(const unsigned char* message,
                                     const std::size_t& message_length) {
    std::array<unsigned char, 32> digest;
    EVP_MD_CTX* message_digest_context = nullptr;
    message_digest_context = EVP_MD_CTX_new();
    unsigned int digest_len;
    if (message_digest_context == nullptr) {
        return digest;
    }
    if (EVP_DigestInit_ex(message_digest_context, EVP_sha256(), nullptr) != 1) {
        return digest;
    }
    if (EVP_DigestUpdate(message_digest_context, message, message_length)
        != 1) {
        return digest;
    }
    if (EVP_DigestFinal_ex(message_digest_context, digest.data(), &digest_len)
        != 1) {
        return digest;
    }
    std::cout << digest_len << std::endl;
    return digest;
}

} // namespace hash
} // namespace crypto
} // namespace utils
} // namespace rayalto
