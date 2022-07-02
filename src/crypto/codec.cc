#include "rautils/crypto/codec.h"

#include <cstddef>
#include <cstring>
#include <vector>

#include "openssl/evp.h"

namespace rayalto::utils::crypto::codec {

std::vector<unsigned char> base64_encode(
    const std::vector<unsigned char>& input_data) {
    return base64_encode(input_data.data(), input_data.size());
}

std::vector<unsigned char> base64_encode(
    const std::vector<unsigned char>& input_data,
    const std::size_t& input_length) {
    return base64_encode(input_data.data(), input_length);
}

std::vector<unsigned char> base64_encode(const char* input_data) {
    return base64_encode(reinterpret_cast<const unsigned char*>(input_data),
                         std::strlen(input_data));
}

std::vector<unsigned char> base64_encode(const char* input_data,
                                         const std::size_t& input_length) {
    return base64_encode(reinterpret_cast<const unsigned char*>(input_data),
                         input_length);
}

std::vector<unsigned char> base64_encode(const unsigned char* input_data) {
    return base64_encode(
        input_data, std::strlen(reinterpret_cast<const char*>(input_data)));
}

std::vector<unsigned char> base64_encode(const unsigned char* input_data,
                                         const std::size_t& input_length) {
    unsigned char result[((input_length / 3) + 2) * 4];
    int result_length =
        EVP_EncodeBlock(result, input_data, static_cast<int>(input_length));
    return {result, result + result_length};
}

std::vector<unsigned char> base64_decode(
    const std::vector<unsigned char>& input_data) {
    return base64_decode(input_data.data(), input_data.size());
}

std::vector<unsigned char> base64_decode(
    const std::vector<unsigned char>& input_data,
    const std::size_t& input_length) {
    return base64_decode(input_data.data(), input_length);
}

std::vector<unsigned char> base64_decode(const char* input_data) {
    return base64_decode(reinterpret_cast<const unsigned char*>(input_data),
                         std::strlen(input_data));
}

std::vector<unsigned char> base64_decode(const char* input_data,
                                         const std::size_t& input_length) {
    return base64_decode(reinterpret_cast<const unsigned char*>(input_data),
                         input_length);
}

std::vector<unsigned char> base64_decode(const unsigned char* input_data) {
    return base64_decode(
        input_data, std::strlen(reinterpret_cast<const char*>(input_data)));
}

std::vector<unsigned char> base64_decode(const unsigned char* input_data,
                                         const std::size_t& input_length) {
    unsigned char result[((input_length / 4) + 1) * 3];
    int result_length =
        EVP_DecodeBlock(result, input_data, static_cast<int>(input_length));
    if (result_length == -1) {
        // failed to decode
        return std::vector<unsigned char> {};
    }
    if (input_length > 3 && input_data[input_length - 1] == '='
        && input_data[input_length - 2] == '=') {
        result_length -= 2;
    }
    else if (input_length > 2 && input_data[input_length - 1] == '=') {
        result_length -= 1;
    }
    return {result, result + result_length};
}

} // namespace rayalto::utils::crypto::codec
