#ifndef RA_UTILS_CRYPTO_CODEC_H_
#define RA_UTILS_CRYPTO_CODEC_H_

#include <cstddef>
#include <vector>

namespace rayalto::utils::crypto::codec {

// base64 encode
std::vector<unsigned char> base64_encode(
    const std::vector<unsigned char>& input_data);
std::vector<unsigned char> base64_encode(
    const std::vector<unsigned char>& input_data,
    const std::size_t& input_length);
std::vector<unsigned char> base64_encode(const char* input_data);
std::vector<unsigned char> base64_encode(const char* input_data,
                                         const std::size_t& input_length);
std::vector<unsigned char> base64_encode(const unsigned char* input_data);
std::vector<unsigned char> base64_encode(const unsigned char* input_data,
                                         const std::size_t& input_length);

// base64 decode, std::vector<unsigned char>::empty() if openssl failed to decode
std::vector<unsigned char> base64_decode(
    const std::vector<unsigned char>& input_data);
std::vector<unsigned char> base64_decode(
    const std::vector<unsigned char>& input_data,
    const std::size_t& input_length);
std::vector<unsigned char> base64_decode(const char* input_data);
std::vector<unsigned char> base64_decode(const char* input_data,
                                         const std::size_t& input_length);
std::vector<unsigned char> base64_decode(const unsigned char* input_data);
std::vector<unsigned char> base64_decode(const unsigned char* input_data,
                                         const std::size_t& input_length);

} // namespace rayalto::utils::crypto::codec

#endif // RA_UTILS_CRYPTO_CODEC_H_
