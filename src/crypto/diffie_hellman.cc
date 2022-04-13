#include "crypto/diffie_hellman.h"

#include <string>

#include "openssl/evp.h"
#include "openssl/ossl_typ.h"

namespace rayalto {
namespace utils {
namespace crypto {

class DiffieHellman::Impl {
public:
    Impl() {}
    Impl(const Impl&) = delete;
    Impl(Impl&&) noexcept = default;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&) noexcept = default;

    virtual ~Impl() {
        if (key_generation_context_ != nullptr) {
            EVP_PKEY_CTX_free(key_generation_context_);
        }
        if (generation_params_ != nullptr) {
            EVP_PKEY_free(generation_params_);
        }
        if (key_pair_ != nullptr) {
            EVP_PKEY_free(key_pair_);
        }
    }

protected:
    EVP_PKEY_CTX* key_generation_context_ = nullptr;
    EVP_PKEY_CTX* key_derivation_context_ = nullptr;
    EVP_PKEY* generation_params_ = nullptr;
    EVP_PKEY* key_pair_ = nullptr;
    std::string peer_public_key_;
    std::string public_key_;
    std::string private_key_;
    std::string param_p_;
    std::string param_q_;
    std::string param_g_;
};

DiffieHellman::~DiffieHellman() = default;

} // namespace crypto
} // namespace utils
} // namespace rayalto
