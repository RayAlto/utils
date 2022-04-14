#include <array>
#include <iomanip>
#include <ios>
#include <iostream>

#include "crypto/codec.h"
#include "crypto/diffie_hellman.h"
#include "crypto/hash.h"
#include "crypto/random.h"
#include "crypto/encrypt.h"
#include "exceptions/exceptions.h"

using namespace rayalto::utils;

int main(int argc, char const *argv[]) {
    // echo '灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n' | openssl dgst -sha256
    std::cout << std::hex;
    for (unsigned char c :
         crypto::hash::sha256("灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n")) {
        std::cout << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(c);
    }
    std::cout << std::oct << std::endl;

    // echo '灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n' | openssl enc -d -base64
    for (unsigned char c : crypto::codec::base64_encode(
             "灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n")) {
        std::cout << static_cast<char>(c);
    }
    std::cout << std::endl;

    std::cout << std::hex;
    for (unsigned char c : crypto::random::bytes(4)) {
        std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    std::cout << std::oct << std::endl;

    std::vector<unsigned char> encrypted = crypto::encrypt::aes256cbc(
        "The quick brown fox jumps over the lazy dog",
        43,
        "01234567890123456789012345678901",
        "0123456789012345");
    std::cout << std::hex;
    for (unsigned char c : encrypted) {
        std::cout << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(c);
    }
    std::cout << std::oct << std::endl;

    std::vector<unsigned char> decrypted = crypto::decrypt::aes256cbc(
        encrypted.data(),
        encrypted.size(),
        reinterpret_cast<const unsigned char *>(
            "01234567890123456789012345678901"),
        reinterpret_cast<const unsigned char *>("0123456789012345"));
    for (unsigned char c : decrypted) {
        std::cout << static_cast<char>(c);
    }
    std::cout << std::endl;

    return 0;
}
