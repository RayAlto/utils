#include <iostream>

#include "rautils/crypto/codec.h"
#include "rautils/crypto/diffie_hellman.h"
#include "rautils/crypto/hash.h"
#include "rautils/crypto/random.h"
#include "rautils/crypto/encrypt.h"

#include "rautils/string/strtool.h"

using namespace rayalto::utils;

int main(int argc, char const *argv[]) {
    // echo '灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n' | openssl dgst -sha256
    std::cout << string::hex_string(
        crypto::hash::sha256("灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n"))
              << std::endl;

    // echo '灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n' | openssl enc -d -base64
    std::cout << string::data_string(
        crypto::codec::base64_encode("灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n"))
              << std::endl;

    std::cout << string::hex_string(crypto::random::bytes(4)) << std::endl;

    std::vector<unsigned char> encrypted =
        crypto::encrypt::aes256cbc("灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！",
                                   54,
                                   "01234567890123456789012345678901",
                                   "0123456789012345");
    std::cout << string::hex_string(encrypted) << std::endl;

    std::vector<unsigned char> decrypted = crypto::decrypt::aes256cbc(
        encrypted.data(),
        encrypted.size(),
        reinterpret_cast<const unsigned char *>(
            "01234567890123456789012345678901"),
        reinterpret_cast<const unsigned char *>("0123456789012345"));
    std::cout << string::data_string(decrypted) << std::endl;

    std::vector<unsigned char> public_key_b64 = string::to_data(
        "AAABAHJuXLnNjrPxIFRN9TL77ML0KSL6iCDRGYPseiyYIahJRgMMrI8Umn5R9zP9gzaM6bpTObK/rOEy6fClj3H5udKDqVsU/71qIf8oo1KadvqwYMz1OpKiFTQlbdarEwbeGjL/C+eZD22kD4xG5VDZFz8tAt2+HMe2waxNna/ERyx1TlOyur7dBSjb0Wlg9kk2R3qufBKSzVz6TWoB+tzDc0wciMEXB6iu5aZutUyd0S9IGoawl/xTSQay2NPDjKIQnnwpj5kYFGVLXAexcpjFH0NedDE/zddM093sW+6wHSsDFfykYYvCqY0VwlawB8N63sAZB50oARyuV2OUQiT4RMQ=");
    std::vector<unsigned char> param_p_b64 = string::to_data(
        "AAABAQCInR/DefsGtJ2uECMbdDVaEOJRB+6cn86I+35udWrHcVzdhiElnMRuSWLdEkTahdJ7IG5k0qKyP02RFch432Km74QOosGsbiMm6XwMoXmH5TwI8wV96+MWhySYoRDrNHbx0ZbQOuLjiiSTh3Ccim/aXNymq4WIvxERKIy7Hg8SRyk3AeR8BoV3eR1ySKRCsGYKNmh1OGtGenyIeAQKkHYqStnivwnodXaCiHSUA/PbFi5Ng+G06JTyGkVHmkduuuTNjsmCB/YjtaGk0BKraQPdcZXsos+cq93S7ajC+ej/ud40OMNFmfXCeh3KuT50AVQPfAlChqOZYQD/P+eV5ofT");
    std::vector<unsigned char> param_q_b64 =
        string::to_data("AAAAIQCHUyuspW42aIvMdWW/srwsY4Hb4PMWg1se3Iwq86zGsQ==");
    std::vector<unsigned char> param_g_b64 = string::to_data(
        "AAABAFOW0YHLmXsnt/PUKIHEuyONMk7URML9idRXsZLrmTUyMxwVWov1h93O4ep5Ue50G5aZ1qo4GbL8aK8Fgp1FMRm4E6zWIp6NDom7+BY3nDZF5TG09d2w8ZzChlH1E1giLacv1riG6WvbS6MWpV4QvQ/B9Pp8ySyeEDCWOcAN7sjcQu7TNj+CfHmYGNFlpMuye8s3CiWz6ocaZ8aj7kiqpN+jORQzgLyQKfd+C6Ai/3mYaDJpF3mBsYrqjsoarX/q8yZnPfTxia/RSnnZZzTTb/JPiOivoPWwWiC9X0IIXwXFpgxy8yWk83xVIwAGPuPG2i1+Z4vyv0k/p9KvHX+dUs4=");
    crypto::DiffieHellman foo;
    foo.param_p(crypto::codec::base64_decode(param_p_b64));
    foo.param_q(crypto::codec::base64_decode(param_q_b64));
    foo.param_g(crypto::codec::base64_decode(param_g_b64));
    foo.peer_public_key(crypto::codec::base64_decode(public_key_b64));
    foo.generate_key_pair();
    foo.derive_shared_key();
    std::cout << "===== shared key" << foo.shared_key().size()
              << " =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.shared_key()))
              << std::endl;
    std::cout << "===== public key =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.public_key()))
              << std::endl;
    std::cout << "===== private key =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.private_key()))
              << std::endl;
    std::cout << "===== param p =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.param_p()))
              << std::endl;
    std::cout << "===== param q =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.param_q()))
              << std::endl;
    std::cout << "===== param g =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.param_g()))
              << std::endl;

    return 0;
}
