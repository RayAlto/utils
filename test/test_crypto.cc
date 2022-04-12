#include <array>
#include <iomanip>
#include <ios>
#include <iostream>

#include "crypto/codec.h"
#include "crypto/hash.h"

using namespace rayalto::utils;

int main(int argc, char const *argv[]) {
    // echo '灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n' | openssl dgst -sha256
    for (unsigned char c :
         crypto::hash::sha256("灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n")) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(c);
    }
    std::cout << std::endl;

    std::cout << crypto::codec::base64_encode(
        "灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n");

    return 0;
}
