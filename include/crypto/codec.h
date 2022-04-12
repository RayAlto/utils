#ifndef RA_UTILS_CRYPTO_CODEC_H_
#define RA_UTILS_CRYPTO_CODEC_H_

#include <cstddef>
#include <string>

namespace rayalto {
namespace utils {
namespace crypto {
namespace codec {

// base64 encode
std::string base64_encode(const std::string& input_data);
std::string base64_encode(const char* input_data);
std::string base64_encode(const unsigned char* input_data);
std::string base64_encode(const std::string& input_data,
                          const std::size_t& input_length);
std::string base64_encode(const char* input_data,
                          const std::size_t& input_length);
std::string base64_encode(const unsigned char* input_data,
                          const std::size_t& input_length);

// base64 decode, std::string::empty() if openssl failed to decode
std::string base64_decode(const std::string& input_data);
std::string base64_decode(const char* input_data);
std::string base64_decode(const unsigned char* input_data);
std::string base64_decode(const std::string& input_data,
                          const std::size_t& input_length);
std::string base64_decode(const char* input_data,
                          const std::size_t& input_length);
std::string base64_decode(const unsigned char* input_data,
                          const std::size_t& input_length);

} // namespace codec
} // namespace crypto
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_CRYPTO_CODEC_H_
