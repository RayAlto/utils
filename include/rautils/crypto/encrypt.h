#ifndef RA_UTILS_CRYPTO_ENCRYPT_H_
#define RA_UTILS_CRYPTO_ENCRYPT_H_

#include <cstddef>
#include <vector>

namespace rayalto::utils::crypto {

namespace encrypt {

/**
 * AES-256 encrypt (CBC mode), key MUST has 256 bits, iv MUST have 128 bits
 *     return empty string if openssl failed
 */
std::vector<unsigned char> aes256cbc(const std::vector<unsigned char>& message,
                                     const std::vector<unsigned char>& key,
                                     const std::vector<unsigned char>& iv);
std::vector<unsigned char> aes256cbc(const std::vector<unsigned char>& message,
                                     const std::size_t& message_length,
                                     const std::vector<unsigned char>& key,
                                     const std::vector<unsigned char>& iv);
std::vector<unsigned char> aes256cbc(const char* message,
                                     const char* key,
                                     const char* iv);
std::vector<unsigned char> aes256cbc(const char* message,
                                     const std::size_t& message_length,
                                     const char* key,
                                     const char* iv);
std::vector<unsigned char> aes256cbc(const unsigned char* message,
                                     const unsigned char* key,
                                     const unsigned char* iv);
std::vector<unsigned char> aes256cbc(const unsigned char* message,
                                     const std::size_t& message_length,
                                     const unsigned char* key,
                                     const unsigned char* iv);

} // namespace encrypt

namespace decrypt {

/**
 * AES-256 encrypt (CBC mode), key MUST has 256 bits, iv MUST have 128 bits
 *     return empty string if openssl failed
 */
std::vector<unsigned char> aes256cbc(
    const std::vector<unsigned char>& ciphertext,
    const std::vector<unsigned char>& key,
    const std::vector<unsigned char>& iv);
std::vector<unsigned char> aes256cbc(
    const std::vector<unsigned char>& ciphertext,
    const std::size_t& ciphertext_length,
    const std::vector<unsigned char>& key,
    const std::vector<unsigned char>& iv);
std::vector<unsigned char> aes256cbc(const char* ciphertext,
                                     const char* key,
                                     const char* iv);
std::vector<unsigned char> aes256cbc(const char* ciphertext,
                                     const std::size_t& ciphertext_length,
                                     const char* key,
                                     const char* iv);
std::vector<unsigned char> aes256cbc(const unsigned char* ciphertext,
                                     const unsigned char* key,
                                     const unsigned char* iv);
std::vector<unsigned char> aes256cbc(const unsigned char* ciphertext,
                                     const std::size_t& ciphertext_length,
                                     const unsigned char* key,
                                     const unsigned char* iv);

} // namespace decrypt

} // namespace rayalto::utils::crypto

#endif // RA_UTILS_CRYPTO_ENCRYPT_H_
