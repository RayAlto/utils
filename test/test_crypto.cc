#include <iostream>

#include "crypto/hash.h"

using namespace rayalto::utils;

int main(int argc, char const *argv[]) {
    // echo '灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n' | openssl dgst -sha256
    for (unsigned char c :
         crypto::hash::sha256("灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n")) {
        std::cout << std::hex << static_cast<unsigned int>(c);
    }
    return 0;
}
