#include "crypto/codec.h"

#include <cstddef>
#include <cstring>
#include <string>

#include "openssl/evp.h"

namespace rayalto {
namespace utils {
namespace crypto {
namespace codec {

std::string base64_encode(const std::string& input_data) {
    return base64_encode(
        reinterpret_cast<const unsigned char*>(input_data.data()),
        input_data.length());
}

std::string base64_encode(const char* input_data) {
    return base64_encode(reinterpret_cast<const unsigned char*>(input_data),
                         std::strlen(input_data));
}

std::string base64_encode(const unsigned char* input_data) {
    return base64_encode(
        input_data, std::strlen(reinterpret_cast<const char*>(input_data)));
}

std::string base64_encode(const std::string& input_data,
                          const std::size_t& input_length) {
    return base64_encode(
        reinterpret_cast<const unsigned char*>(input_data.data()),
        input_length);
}

std::string base64_encode(const char* input_data,
                          const std::size_t& input_length) {
    return base64_encode(reinterpret_cast<const unsigned char*>(input_data),
                         input_length);
}

std::string base64_encode(const unsigned char* input_data,
                          const std::size_t& input_length) {
    char result[((input_length / 3) + 2) * 4];
    int result_length = EVP_EncodeBlock(
        reinterpret_cast<unsigned char*>(result), input_data, input_length);
    return std::string(result, result_length);
}

std::string base64_decode(const std::string& input_data) {
    return base64_decode(
        reinterpret_cast<const unsigned char*>(input_data.data()),
        input_data.length());
}

std::string base64_decode(const char* input_data) {
    return base64_decode(reinterpret_cast<const unsigned char*>(input_data),
                         std::strlen(input_data));
}

std::string base64_decode(const unsigned char* input_data) {
    return base64_decode(
        input_data, std::strlen(reinterpret_cast<const char*>(input_data)));
}

std::string base64_decode(const std::string& input_data,
                          const std::size_t& input_length) {
    return base64_decode(
        reinterpret_cast<const unsigned char*>(input_data.data()),
        input_length);
}

std::string base64_decode(const char* input_data,
                          const std::size_t& input_length) {
    return base64_decode(reinterpret_cast<const unsigned char*>(input_data),
                         input_length);
}

std::string base64_decode(const unsigned char* input_data,
                          const std::size_t& input_length) {
    char result[((input_length / 4) + 1) * 3];
    int result_length = EVP_DecodeBlock(
        reinterpret_cast<unsigned char*>(result), input_data, input_length);
    if (result_length == -1) {
        // failed to decode
        return std::string {};
    }
    return std::string(result, result_length);
}

} // namespace codec
} // namespace crypto
} // namespace utils
} // namespace rayalto
