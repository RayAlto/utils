#include "crypto/hash.h"

#include <array>
#include <cstddef>
#include <cstring>
#include <string>

#include "openssl/crypto.h"
#include "openssl/evp.h"
#include "openssl/ossl_typ.h"

namespace rayalto {
namespace utils {
namespace crypto {
namespace hash {

std::string sha256(const std::string& message) {
    return sha256(reinterpret_cast<const unsigned char*>(message.data()),
                  message.length());
}

std::string sha256(const std::string& message,
                   const std::size_t& message_length) {
    return sha256(reinterpret_cast<const unsigned char*>(message.data()),
                  message_length);
}

std::string sha256(const char* message) {
    return sha256(reinterpret_cast<const unsigned char*>(message),
                  std::strlen(message));
}

std::string sha256(const char* message, const std::size_t& message_length) {
    return sha256(reinterpret_cast<const unsigned char*>(message),
                  message_length);
}

std::string sha256(const unsigned char* message) {
    return sha256(message, std::strlen(reinterpret_cast<const char*>(message)));
}

std::string sha256(const unsigned char* message,
                   const std::size_t& message_length) {
    std::string result;
    EVP_MD_CTX* message_digest_context = nullptr;
    unsigned char* digest = nullptr;
    unsigned int digest_len = 0;
    message_digest_context = EVP_MD_CTX_new();
    if (message_digest_context == nullptr) {
        EVP_MD_CTX_free(message_digest_context);
        return result;
    }
    if (EVP_DigestInit_ex(message_digest_context, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(message_digest_context);
        return result;
    }
    if (EVP_DigestUpdate(message_digest_context, message, message_length)
        != 1) {
        EVP_MD_CTX_free(message_digest_context);
        return result;
    }
    digest = reinterpret_cast<unsigned char*>(
        OPENSSL_malloc(EVP_MD_size(EVP_sha256())));
    if (digest == nullptr) {
        EVP_MD_CTX_free(message_digest_context);
        OPENSSL_free(digest);
        return result;
    }
    if (EVP_DigestFinal_ex(message_digest_context, digest, &digest_len) != 1) {
        EVP_MD_CTX_free(message_digest_context);
        OPENSSL_free(digest);
        return result;
    }
    result = std::string(reinterpret_cast<char*>(digest), digest_len);
    EVP_MD_CTX_free(message_digest_context);
    OPENSSL_free(digest);
    return result;
}

} // namespace hash
} // namespace crypto
} // namespace utils
} // namespace rayalto
