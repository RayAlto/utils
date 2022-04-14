#include "crypto/encrypt.h"

#include <cstddef>
#include <cstring>
#include <string>

#include "openssl/evp.h"
#include "openssl/ossl_typ.h"

namespace rayalto {
namespace utils {
namespace crypto {

namespace encrypt {

std::string aes256cbc(const std::string& message,
                      const std::string& key,
                      const std::string& iv) {
    return aes256cbc(reinterpret_cast<const unsigned char*>(message.data()),
                     message.length(),
                     reinterpret_cast<const unsigned char*>(key.data()),
                     reinterpret_cast<const unsigned char*>(iv.data()));
}

std::string aes256cbc(const std::string& message,
                      const std::size_t& message_length,
                      const std::string& key,
                      const std::string& iv) {
    return aes256cbc(reinterpret_cast<const unsigned char*>(message.data()),
                     message_length,
                     reinterpret_cast<const unsigned char*>(key.data()),
                     reinterpret_cast<const unsigned char*>(iv.data()));
}

std::string aes256cbc(const char* message, const char* key, const char* iv) {
    return aes256cbc(reinterpret_cast<const unsigned char*>(message),
                     std::strlen(message),
                     reinterpret_cast<const unsigned char*>(key),
                     reinterpret_cast<const unsigned char*>(iv));
}

std::string aes256cbc(const char* message,
                      const std::size_t& message_length,
                      const char* key,
                      const char* iv) {
    return aes256cbc(reinterpret_cast<const unsigned char*>(message),
                     message_length,
                     reinterpret_cast<const unsigned char*>(key),
                     reinterpret_cast<const unsigned char*>(iv));
}

std::string aes256cbc(const unsigned char* message,
                      const unsigned char* key,
                      const unsigned char* iv) {
    return aes256cbc(
        message, std::strlen(reinterpret_cast<const char*>(message)), key, iv);
}

std::string aes256cbc(const unsigned char* message,
                      const std::size_t& message_length,
                      const unsigned char* key,
                      const unsigned char* iv) {
    std::string result;
    int buffer_length;
    int result_length;
    unsigned char
        buffer[message_length + EVP_CIPHER_block_size(EVP_aes_256_cbc())];
    EVP_CIPHER_CTX* cipher_context = nullptr;
    cipher_context = EVP_CIPHER_CTX_new();
    if (cipher_context == nullptr) {
        EVP_CIPHER_CTX_free(cipher_context);
        return result;
    }
    if (EVP_EncryptInit_ex(cipher_context, EVP_aes_256_cbc(), nullptr, key, iv)
        != 1) {
        EVP_CIPHER_CTX_free(cipher_context);
        return result;
    }
    if (EVP_EncryptUpdate(
            cipher_context, buffer, &buffer_length, message, message_length)
        != 1) {
        EVP_CIPHER_CTX_free(cipher_context);
        return result;
    }
    result_length = buffer_length;
    if (EVP_EncryptFinal_ex(
            cipher_context, buffer + buffer_length, &buffer_length)
        != 1) {
        EVP_CIPHER_CTX_free(cipher_context);
        return result;
    }
    result_length += buffer_length;
    result = std::string(reinterpret_cast<char*>(buffer), result_length);
    EVP_CIPHER_CTX_free(cipher_context);
    return result;
}

} // namespace encrypt

namespace decrypt {

std::string aes256cbc(const std::string& ciphertext,
                      const std::string& key,
                      const std::string& iv) {
    return aes256cbc(reinterpret_cast<const unsigned char*>(ciphertext.data()),
                     ciphertext.length(),
                     reinterpret_cast<const unsigned char*>(key.data()),
                     reinterpret_cast<const unsigned char*>(iv.data()));
}

std::string aes256cbc(const std::string& ciphertext,
                      const std::size_t& ciphertext_length,
                      const std::string& key,
                      const std::string& iv) {
    return aes256cbc(reinterpret_cast<const unsigned char*>(ciphertext.data()),
                     ciphertext_length,
                     reinterpret_cast<const unsigned char*>(key.data()),
                     reinterpret_cast<const unsigned char*>(iv.data()));
}

std::string aes256cbc(const char* ciphertext, const char* key, const char* iv) {
    return aes256cbc(reinterpret_cast<const unsigned char*>(ciphertext),
                     std::strlen(ciphertext),
                     reinterpret_cast<const unsigned char*>(key),
                     reinterpret_cast<const unsigned char*>(iv));
}

std::string aes256cbc(const char* ciphertext,
                      const std::size_t& ciphertext_length,
                      const char* key,
                      const char* iv) {
    return aes256cbc(reinterpret_cast<const unsigned char*>(ciphertext),
                     ciphertext_length,
                     reinterpret_cast<const unsigned char*>(key),
                     reinterpret_cast<const unsigned char*>(iv));
}

std::string aes256cbc(const unsigned char* ciphertext,
                      const unsigned char* key,
                      const unsigned char* iv) {
    return aes256cbc(ciphertext,
                     std::strlen(reinterpret_cast<const char*>(ciphertext)),
                     key,
                     iv);
}

std::string aes256cbc(const unsigned char* ciphertext,
                      const std::size_t& ciphertext_length,
                      const unsigned char* key,
                      const unsigned char* iv) {
    std::string result;
    int buffer_length;
    int result_length;
    unsigned char
        buffer[ciphertext_length + EVP_CIPHER_block_size(EVP_aes_256_cbc())];
    EVP_CIPHER_CTX* cipher_context = nullptr;
    cipher_context = EVP_CIPHER_CTX_new();
    if (cipher_context == nullptr) {
        EVP_CIPHER_CTX_free(cipher_context);
        return result;
    }
    if (EVP_DecryptInit_ex(cipher_context, EVP_aes_256_cbc(), nullptr, key, iv)
        != 1) {
        EVP_CIPHER_CTX_free(cipher_context);
        return result;
    }
    if (EVP_DecryptUpdate(cipher_context,
                          buffer,
                          &buffer_length,
                          ciphertext,
                          ciphertext_length)
        != 1) {
        EVP_CIPHER_CTX_free(cipher_context);
        return result;
    }
    result_length = buffer_length;
    if (EVP_DecryptFinal_ex(
            cipher_context, buffer + buffer_length, &buffer_length)
        != 1) {
        EVP_CIPHER_CTX_free(cipher_context);
        return result;
    }
    result_length += buffer_length;
    result = std::string(reinterpret_cast<char*>(buffer), result_length);
    EVP_CIPHER_CTX_free(cipher_context);
    return result;
}

} // namespace decrypt

} // namespace crypto
} // namespace utils
} // namespace rayalto