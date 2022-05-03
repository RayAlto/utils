#include "rautils/crypto/diffie_hellman.h"

#include <cstddef>
#include <cstring>
#include <memory>
#include <utility>
#include <vector>

#include "openssl/bn.h"
#include "openssl/crypto.h"
#include "openssl/dh.h"
#include "openssl/evp.h"
#include "openssl/ossl_typ.h"

#include "rautils/exceptions/exceptions.h"

namespace rayalto {
namespace utils {
namespace crypto {

class DiffieHellman::Impl {
public:
    Impl() = default;
    Impl(const Impl&) = delete;
    Impl(Impl&&) noexcept = default;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&) noexcept = default;

    virtual ~Impl();

    void peer_public_key(const std::vector<unsigned char>& peer_public_key);
    void peer_public_key(const std::vector<unsigned char>& peer_public_key,
                         const std::size_t& peer_public_key_length);
    void peer_public_key(std::vector<unsigned char>&& peer_public_key);
    void peer_public_key(std::vector<unsigned char>&& peer_public_key,
                         const std::size_t& peer_public_key_length);
    void peer_public_key(const char* peer_public_key);
    void peer_public_key(const char* peer_public_key,
                         const std::size_t& peer_public_key_length);
    void peer_public_key(const unsigned char* peer_public_key);
    void peer_public_key(const unsigned char* peer_public_key,
                         const std::size_t& peer_public_key_length);
    std::vector<unsigned char>& peer_public_key();
    const std::vector<unsigned char>& peer_public_key() const;

    void param_p(const std::vector<unsigned char>& param_p);
    void param_p(const std::vector<unsigned char>& param_p,
                 const std::size_t& param_p_length);
    void param_p(std::vector<unsigned char>&& param_p);
    void param_p(std::vector<unsigned char>&& param_p,
                 const std::size_t& param_p_length);
    void param_p(const char* param_p);
    void param_p(const char* param_p, const std::size_t& param_p_length);
    void param_p(const unsigned char* param_p);
    void param_p(const unsigned char* param_p,
                 const std::size_t& param_p_length);
    std::vector<unsigned char>& param_p();
    const std::vector<unsigned char>& param_p() const;

    void param_q(const std::vector<unsigned char>& param_q);
    void param_q(const std::vector<unsigned char>& param_q,
                 const std::size_t& param_q_length);
    void param_q(std::vector<unsigned char>&& param_q);
    void param_q(std::vector<unsigned char>&& param_q,
                 const std::size_t& param_q_length);
    void param_q(const char* param_q);
    void param_q(const char* param_q, const std::size_t& param_q_length);
    void param_q(const unsigned char* param_q);
    void param_q(const unsigned char* param_q,
                 const std::size_t& param_q_length);
    std::vector<unsigned char>& param_q();
    const std::vector<unsigned char>& param_q() const;

    void param_g(const std::vector<unsigned char>& param_g);
    void param_g(const std::vector<unsigned char>& param_g,
                 const std::size_t& param_g_length);
    void param_g(std::vector<unsigned char>&& param_g);
    void param_g(std::vector<unsigned char>&& param_g,
                 const std::size_t& param_g_length);
    void param_g(const char* param_g);
    void param_g(const char* param_g, const std::size_t& param_g_length);
    void param_g(const unsigned char* param_g);
    void param_g(const unsigned char* param_g,
                 const std::size_t& param_g_length);
    std::vector<unsigned char>& param_g();
    const std::vector<unsigned char>& param_g() const;

    const std::vector<unsigned char>& public_key() const;
    const std::vector<unsigned char>& private_key() const;
    const std::vector<unsigned char>& shared_key() const;

    void use_built_in_params(const bool& built_in);
    void use_built_in_params(bool&& built_in);

    const bool& use_built_in_params() const;
    bool& use_built_in_params();

    void generate_key_pair();

    void derive_shared_key();

protected:
    EVP_PKEY_CTX* key_generation_context_ = nullptr;
    EVP_PKEY_CTX* key_derivation_context_ = nullptr;
    EVP_PKEY* generation_params_ = nullptr;
    EVP_PKEY* key_pair_ = nullptr;
    EVP_PKEY* peer_key_ = nullptr;
    std::vector<unsigned char> peer_public_key_;
    std::vector<unsigned char> public_key_;
    std::vector<unsigned char> private_key_;
    std::vector<unsigned char> param_p_;
    std::vector<unsigned char> param_q_;
    std::vector<unsigned char> param_g_;
    std::vector<unsigned char> shared_key_;
    bool use_built_in_params_ = false;

    void init_params_();
    void init_built_in_params_();
    void init_generated_params_();
    void init_custom_params_();
    void clear_params_();

    void init_generation_context_();
    void clear_generation_context_();

    void generate_key_pair_();
    void clear_key_pair_();

    void init_peer_key_();
    void clear_peer_key_();

    void init_derivation_context_();
    void clear_derivation_context_();

    void derive_shared_key_();

    void store_key_pair_();
    void store_params_();
};

DiffieHellman::Impl::~Impl() {
    clear_generation_context_();
    clear_params_();
    clear_key_pair_();
    clear_peer_key_();
    clear_derivation_context_();
}

void DiffieHellman::Impl::peer_public_key(
    const std::vector<unsigned char>& peer_public_key) {
    peer_public_key_ = peer_public_key;
}

void DiffieHellman::Impl::peer_public_key(
    const std::vector<unsigned char>& peer_public_key,
    const std::size_t& peer_public_key_length) {
    peer_public_key_ = std::vector<unsigned char>(
        peer_public_key.begin(),
        peer_public_key.begin() + peer_public_key_length);
}

void DiffieHellman::Impl::peer_public_key(
    std::vector<unsigned char>&& peer_public_key) {
    peer_public_key_ = std::move(peer_public_key);
}

void DiffieHellman::Impl::peer_public_key(
    std::vector<unsigned char>&& peer_public_key,
    const std::size_t& peer_public_key_length) {
    peer_public_key.erase(peer_public_key.begin() + peer_public_key_length,
                          peer_public_key.end());
    peer_public_key_ = std::move(peer_public_key);
}

void DiffieHellman::Impl::peer_public_key(const char* peer_public_key) {
    const unsigned char* peer_public_key_begin =
        reinterpret_cast<const unsigned char*>(peer_public_key);
    peer_public_key_ = std::vector<unsigned char>(
        peer_public_key_begin,
        peer_public_key_begin + std::strlen(peer_public_key));
}

void DiffieHellman::Impl::peer_public_key(
    const char* peer_public_key, const std::size_t& peer_public_key_length) {
    const unsigned char* peer_public_key_begin =
        reinterpret_cast<const unsigned char*>(peer_public_key);
    peer_public_key_ = std::vector<unsigned char>(
        peer_public_key_begin, peer_public_key_begin + peer_public_key_length);
}

void DiffieHellman::Impl::peer_public_key(
    const unsigned char* peer_public_key) {
    peer_public_key_ = std::vector<unsigned char>(
        peer_public_key,
        peer_public_key
            + std::strlen(reinterpret_cast<const char*>(peer_public_key)));
}

void DiffieHellman::Impl::peer_public_key(
    const unsigned char* peer_public_key,
    const std::size_t& peer_public_key_length) {
    peer_public_key_ = std::vector<unsigned char>(
        peer_public_key, peer_public_key + peer_public_key_length);
}

std::vector<unsigned char>& DiffieHellman::Impl::peer_public_key() {
    return peer_public_key_;
}

const std::vector<unsigned char>& DiffieHellman::Impl::peer_public_key() const {
    return peer_public_key_;
}

void DiffieHellman::Impl::param_p(const std::vector<unsigned char>& param_p) {
    param_p_ = param_p;
}

void DiffieHellman::Impl::param_p(const std::vector<unsigned char>& param_p,
                                  const std::size_t& param_p_length) {
    param_p_ = std::vector<unsigned char>(param_p.begin(),
                                          param_p.begin() + param_p_length);
}

void DiffieHellman::Impl::param_p(std::vector<unsigned char>&& param_p) {
    param_p_ = std::move(param_p);
}

void DiffieHellman::Impl::param_p(std::vector<unsigned char>&& param_p,
                                  const std::size_t& param_p_length) {
    param_p.erase(param_p.begin() + param_p_length, param_p.end());
    param_p_ = std::move(param_p);
}

void DiffieHellman::Impl::param_p(const char* param_p) {
    const unsigned char* param_p_begin =
        reinterpret_cast<const unsigned char*>(param_p);
    param_p_ = std::vector<unsigned char>(param_p_begin,
                                          param_p_begin + std::strlen(param_p));
}

void DiffieHellman::Impl::param_p(const char* param_p,
                                  const std::size_t& param_p_length) {
    const unsigned char* param_p_begin =
        reinterpret_cast<const unsigned char*>(param_p);
    param_p_ = std::vector<unsigned char>(param_p_begin,
                                          param_p_begin + param_p_length);
}

void DiffieHellman::Impl::param_p(const unsigned char* param_p) {
    param_p_ = std::vector<unsigned char>(
        param_p, param_p + std::strlen(reinterpret_cast<const char*>(param_p)));
}

void DiffieHellman::Impl::param_p(const unsigned char* param_p,
                                  const std::size_t& param_p_length) {
    param_p_ = std::vector<unsigned char>(param_p, param_p + param_p_length);
}

std::vector<unsigned char>& DiffieHellman::Impl::param_p() {
    return param_p_;
}

const std::vector<unsigned char>& DiffieHellman::Impl::param_p() const {
    return param_p_;
}

void DiffieHellman::Impl::param_q(const std::vector<unsigned char>& param_q) {
    param_q_ = param_q;
}

void DiffieHellman::Impl::param_q(const std::vector<unsigned char>& param_q,
                                  const std::size_t& param_q_length) {
    param_q_ = std::vector<unsigned char>(param_q.begin(),
                                          param_q.begin() + param_q_length);
}

void DiffieHellman::Impl::param_q(std::vector<unsigned char>&& param_q) {
    param_q_ = std::move(param_q);
}

void DiffieHellman::Impl::param_q(std::vector<unsigned char>&& param_q,
                                  const std::size_t& param_q_length) {
    param_q.erase(param_q.begin() + param_q_length, param_q.end());
    param_q_ = std::move(param_q);
}

void DiffieHellman::Impl::param_q(const char* param_q) {
    const unsigned char* param_q_begin =
        reinterpret_cast<const unsigned char*>(param_q);
    param_q_ = std::vector<unsigned char>(param_q_begin,
                                          param_q_begin + std::strlen(param_q));
}

void DiffieHellman::Impl::param_q(const char* param_q,
                                  const std::size_t& param_q_length) {
    const unsigned char* param_q_begin =
        reinterpret_cast<const unsigned char*>(param_q);
    param_q_ = std::vector<unsigned char>(param_q_begin,
                                          param_q_begin + param_q_length);
}

void DiffieHellman::Impl::param_q(const unsigned char* param_q) {
    param_q_ = std::vector<unsigned char>(
        param_q, param_q + std::strlen(reinterpret_cast<const char*>(param_q)));
}

void DiffieHellman::Impl::param_q(const unsigned char* param_q,
                                  const std::size_t& param_q_length) {
    param_q_ = std::vector<unsigned char>(param_q, param_q + param_q_length);
}

std::vector<unsigned char>& DiffieHellman::Impl::param_q() {
    return param_q_;
}

const std::vector<unsigned char>& DiffieHellman::Impl::param_q() const {
    return param_q_;
}

void DiffieHellman::Impl::param_g(const std::vector<unsigned char>& param_g) {
    param_g_ = param_g;
}

void DiffieHellman::Impl::param_g(const std::vector<unsigned char>& param_g,
                                  const std::size_t& param_g_length) {
    param_g_ = std::vector<unsigned char>(param_g.begin(),
                                          param_g.begin() + param_g_length);
}

void DiffieHellman::Impl::param_g(std::vector<unsigned char>&& param_g) {
    param_g_ = std::move(param_g);
}

void DiffieHellman::Impl::param_g(std::vector<unsigned char>&& param_g,
                                  const std::size_t& param_g_length) {
    param_g.erase(param_g.begin() + param_g_length, param_g.end());
    param_g_ = std::move(param_g);
}

void DiffieHellman::Impl::param_g(const char* param_g) {
    const unsigned char* param_g_begin =
        reinterpret_cast<const unsigned char*>(param_g);
    param_g_ = std::vector<unsigned char>(param_g_begin,
                                          param_g_begin + std::strlen(param_g));
}

void DiffieHellman::Impl::param_g(const char* param_g,
                                  const std::size_t& param_g_length) {
    const unsigned char* param_g_begin =
        reinterpret_cast<const unsigned char*>(param_g);
    param_g_ = std::vector<unsigned char>(param_g_begin,
                                          param_g_begin + param_g_length);
}

void DiffieHellman::Impl::param_g(const unsigned char* param_g) {
    param_g_ = std::vector<unsigned char>(
        param_g, param_g + std::strlen(reinterpret_cast<const char*>(param_g)));
}

void DiffieHellman::Impl::param_g(const unsigned char* param_g,
                                  const std::size_t& param_g_length) {
    param_g_ = std::vector<unsigned char>(param_g, param_g + param_g_length);
}

std::vector<unsigned char>& DiffieHellman::Impl::param_g() {
    return param_g_;
}

const std::vector<unsigned char>& DiffieHellman::Impl::param_g() const {
    return param_g_;
}

const std::vector<unsigned char>& DiffieHellman::Impl::public_key() const {
    return public_key_;
}

const std::vector<unsigned char>& DiffieHellman::Impl::private_key() const {
    return private_key_;
}

const std::vector<unsigned char>& DiffieHellman::Impl::shared_key() const {
    return shared_key_;
}

void DiffieHellman::Impl::use_built_in_params(const bool& built_in) {
    use_built_in_params_ = built_in;
}

void DiffieHellman::Impl::use_built_in_params(bool&& built_in) {
    use_built_in_params_ = std::move(built_in);
}

const bool& DiffieHellman::Impl::use_built_in_params() const {
    return use_built_in_params_;
}

bool& DiffieHellman::Impl::use_built_in_params() {
    return use_built_in_params_;
}

void DiffieHellman::Impl::generate_key_pair() {
    init_params_();
    init_generation_context_();
    generate_key_pair_();
}

void DiffieHellman::Impl::derive_shared_key() {
    init_derivation_context_();
    derive_shared_key_();
}

void DiffieHellman::Impl::init_params_() {
    if (generation_params_ != nullptr) {
        // already initialized
        return;
    }

    if (use_built_in_params_) {
        // use OpenSSL built-in params
        init_built_in_params_();
        return;
    }

    if (param_p_.empty() || param_q_.empty() || param_g_.empty()) {
        // not enougn params, generate new params
        init_generated_params_();
    }
    else {
        // use params set by user
        init_custom_params_();
    }
}

void DiffieHellman::Impl::init_built_in_params_() {
    // allocate
    generation_params_ = EVP_PKEY_new();
    if (generation_params_ == nullptr) {
        throw exceptions::OpensslError("EVP_PKEY_new()");
    }

    // pass OpenSSL built-in params to key generation context
    if (EVP_PKEY_assign(generation_params_, EVP_PKEY_DHX, DH_get_2048_256())
        != 1) {
        throw exceptions::OpensslError("EVP_PKEY_assign()");
    }
}

void DiffieHellman::Impl::init_generated_params_() {
    // init params generation context
    EVP_PKEY_CTX* params_generation_context =
        EVP_PKEY_CTX_new_id(EVP_PKEY_DH, nullptr);
    if (params_generation_context == nullptr) {
        throw exceptions::OpensslError("EVP_PKEY_CTX_new_id()");
    }
    if (EVP_PKEY_paramgen_init(params_generation_context) != 1) {
        throw exceptions::OpensslError("EVP_PKEY_paramgen_init()");
    }

    // set params generation properties
    if (EVP_PKEY_CTX_set_dh_paramgen_prime_len(params_generation_context, 2048)
        <= 0) {
        throw exceptions::OpensslError(
            "EVP_PKEY_CTX_set_dh_paramgen_prime_len()");
    }
    if (EVP_PKEY_CTX_set_dh_paramgen_type(params_generation_context, 1) <= 0) {
        throw exceptions::OpensslError("EVP_PKEY_CTX_set_dh_paramgen_type()");
    }
    if (EVP_PKEY_CTX_set_dh_paramgen_subprime_len(params_generation_context,
                                                  256)
        <= 0) {
        throw exceptions::OpensslError(
            "EVP_PKEY_CTX_set_dh_paramgen_subprime_len()");
    }

    // generate key generation params
    if (EVP_PKEY_paramgen(params_generation_context, &generation_params_)
        != 1) {
        throw exceptions::OpensslError("EVP_PKEY_paramgen()");
    }
    EVP_PKEY_CTX_free(params_generation_context);
    store_params_();
}

void DiffieHellman::Impl::init_custom_params_() {
    // allocate
    generation_params_ = EVP_PKEY_new();
    if (generation_params_ == nullptr) {
        throw exceptions::OpensslError("EVP_PKEY_new()");
    }

    // init param DH
    DH* param_dh = DH_new();
    if (param_dh == nullptr) {
        throw exceptions::OpensslError("DH_new()");
    }
    BIGNUM* param_p_bn = BN_mpi2bn(param_p_.data(), param_p_.size(), nullptr);
    if (param_p_bn == nullptr) {
        throw exceptions::OpensslError("BN_mpi2bn()");
    }
    BIGNUM* param_q_bn = BN_mpi2bn(param_q_.data(), param_q_.size(), nullptr);
    if (param_q_bn == nullptr) {
        throw exceptions::OpensslError("BN_mpi2bn()");
    }
    BIGNUM* param_g_bn = BN_mpi2bn(param_g_.data(), param_g_.size(), nullptr);
    if (param_g_bn == nullptr) {
        throw exceptions::OpensslError("BN_mpi2bn()");
    }
    if (DH_set0_pqg(param_dh, param_p_bn, param_q_bn, param_g_bn) == 0) {
        throw exceptions::OpensslError("DH_set0_pqg()");
    }

    if (EVP_PKEY_assign(generation_params_, EVP_PKEY_DHX, param_dh) == 0) {
        throw exceptions::OpensslError("EVP_PKEY_assign()");
    }
}

void DiffieHellman::Impl::clear_params_() {
    if (generation_params_ == nullptr) {
        // not allocated
        return;
    }
    EVP_PKEY_free(generation_params_);
    generation_params_ = nullptr;
}

void DiffieHellman::Impl::init_generation_context_() {
    if (generation_params_ == nullptr) {
        // initialize generation params first
        init_params_();
    }

    key_generation_context_ = EVP_PKEY_CTX_new(generation_params_, nullptr);
    if (key_generation_context_ == nullptr) {
        throw exceptions::OpensslError("EVP_PKEY_CTX_new()");
    }
    if (EVP_PKEY_keygen_init(key_generation_context_) != 1) {
        throw exceptions::OpensslError("EVP_PKEY_keygen_init()");
    }
}

void DiffieHellman::Impl::clear_generation_context_() {
    if (key_generation_context_ == nullptr) {
        // not allocated
        return;
    }
    EVP_PKEY_CTX_free(key_generation_context_);
    key_generation_context_ = nullptr;
}

void DiffieHellman::Impl::generate_key_pair_() {
    if (key_generation_context_ == nullptr) {
        // initialize generation context first
        init_generation_context_();
    }

    if (EVP_PKEY_keygen(key_generation_context_, &key_pair_) != 1) {
        throw exceptions::OpensslError("EVP_PKEY_keygen()");
    }
    store_key_pair_();
}

void DiffieHellman::Impl::clear_key_pair_() {
    if (key_pair_ == nullptr) {
        // not allocated
        return;
    }
    EVP_PKEY_free(key_pair_);
    key_pair_ = nullptr;
}

void DiffieHellman::Impl::init_peer_key_() {
    // alocate peer PKEY
    peer_key_ = EVP_PKEY_new();
    if (peer_key_ == nullptr) {
        throw exceptions::OpensslError("EVP_PKEY_new()");
    }

    // allocate peer DH
    DH* peer_dh = DH_new();
    if (peer_dh == nullptr) {
        throw exceptions::OpensslError("DH_new()");
    }

    // set peer public key
    BIGNUM* peer_public_key_bn =
        BN_mpi2bn(peer_public_key_.data(), peer_public_key_.size(), nullptr);
    if (peer_public_key_bn == nullptr) {
        throw exceptions::OpensslError("BN_mpi2bn()");
    }
    DH_set0_key(peer_dh, peer_public_key_bn, nullptr);

    // set peer params (same with self)
    DH* self_dh = EVP_PKEY_get0_DH(key_pair_);
    if (self_dh == nullptr) {
        throw exceptions::OpensslError("DH_new()");
    }
    BIGNUM* param_p_bn = BN_dup(DH_get0_p(self_dh));
    if (param_p_bn == nullptr) {
        throw exceptions::OpensslError("BN_mpi2bn()");
    }
    BIGNUM* param_q_bn = BN_dup(DH_get0_q(self_dh));
    if (param_q_bn == nullptr) {
        throw exceptions::OpensslError("BN_mpi2bn()");
    }
    BIGNUM* param_g_bn = BN_dup(DH_get0_g(self_dh));
    if (param_g_bn == nullptr) {
        throw exceptions::OpensslError("BN_mpi2bn()");
    }
    if (DH_set0_pqg(peer_dh, param_p_bn, param_q_bn, param_g_bn) == 0) {
        throw exceptions::OpensslError("DH_set0_pqg()");
    }

    // pass DH to peer PKEY
    if (EVP_PKEY_assign(peer_key_, EVP_PKEY_DHX, peer_dh) != 1) {
        throw exceptions::OpensslError("EVP_PKEY_assign()");
    }
}

void DiffieHellman::Impl::clear_peer_key_() {
    if (peer_key_ == nullptr) {
        // not allocated
        return;
    }
    EVP_PKEY_free(peer_key_);
    peer_key_ = nullptr;
}

void DiffieHellman::Impl::init_derivation_context_() {
    if (peer_key_ == nullptr) {
        // init peer key first
        init_peer_key_();
    }

    key_derivation_context_ = EVP_PKEY_CTX_new(key_pair_, nullptr);
    if (key_derivation_context_ == nullptr) {
        throw exceptions::OpensslError("EVP_PKEY_CTX_new()");
    }
    if (EVP_PKEY_derive_init(key_derivation_context_) != 1) {
        throw exceptions::OpensslError("EVP_PKEY_derive_init()");
    }

    // pass peer to key derivation context
    if (EVP_PKEY_derive_set_peer(key_derivation_context_, peer_key_) != 1) {
        throw exceptions::OpensslError("EVP_PKEY_derive_set_peer()");
    }
}

void DiffieHellman::Impl::clear_derivation_context_() {
    if (key_derivation_context_ == nullptr) {
        // not allocated
        return;
    }
    EVP_PKEY_CTX_free(key_derivation_context_);
    key_derivation_context_ = nullptr;
}

void DiffieHellman::Impl::derive_shared_key_() {
    // calculate length of shared key
    std::size_t shared_key_length;
    if (EVP_PKEY_derive(key_derivation_context_, nullptr, &shared_key_length)
        != 1) {
        throw exceptions::OpensslError("EVP_PKEY_derive()");
    }

    // store shared key
    unsigned char* shared_key =
        reinterpret_cast<unsigned char*>(OPENSSL_malloc(shared_key_length));
    if (shared_key == nullptr) {
        throw exceptions::OpensslError("OPENSSL_malloc()");
    }
    if (EVP_PKEY_derive(key_derivation_context_, shared_key, &shared_key_length)
        != 1) {
        throw exceptions::OpensslError("EVP_PKEY_derive()");
    }
    shared_key_ =
        std::vector<unsigned char>(shared_key, shared_key + shared_key_length);
    OPENSSL_free(shared_key);
}

void DiffieHellman::Impl::store_key_pair_() {
    DH* key_pair_dh = EVP_PKEY_get0_DH(key_pair_);
    if (key_pair_dh == nullptr) {
        throw exceptions::OpensslError("EVP_PKEY_get0_DH()");
    }

    // public key
    const BIGNUM* public_key_bn = DH_get0_pub_key(key_pair_dh);
    if (public_key_bn == nullptr) {
        throw exceptions::OpensslError("DH_get0_pub_key()");
    }
    std::size_t public_key_data_length = BN_bn2mpi(public_key_bn, nullptr);
    unsigned char public_key_data[public_key_data_length];
    BN_bn2mpi(public_key_bn, public_key_data);
    public_key_ = std::vector<unsigned char>(
        public_key_data, public_key_data + public_key_data_length);

    // private key
    const BIGNUM* private_key_bn = DH_get0_priv_key(key_pair_dh);
    if (private_key_bn == nullptr) {
        throw exceptions::OpensslError("DH_get0_priv_key()");
    }
    std::size_t private_key_data_length = BN_bn2mpi(private_key_bn, nullptr);
    unsigned char private_key_data[private_key_data_length];
    BN_bn2mpi(private_key_bn, private_key_data);
    private_key_ = std::vector<unsigned char>(
        private_key_data, public_key_data + private_key_data_length);
}

void DiffieHellman::Impl::store_params_() {
    DH* params_dh = EVP_PKEY_get0_DH(generation_params_);
    if (params_dh == nullptr) {
        throw exceptions::OpensslError("DH_get0_DH()");
    }

    // param p
    const BIGNUM* param_p_bn = DH_get0_p(params_dh);
    if (param_p_bn == nullptr) {
        throw exceptions::OpensslError("DH_get0_p()");
    }
    std::size_t param_p_data_length = BN_bn2mpi(param_p_bn, nullptr);
    unsigned char param_p_data[param_p_data_length];
    BN_bn2mpi(param_p_bn, param_p_data);
    param_p_ = std::vector<unsigned char>(param_p_data,
                                          param_p_data + param_p_data_length);

    // param q
    const BIGNUM* param_q_bn = DH_get0_q(params_dh);
    if (param_q_bn == nullptr) {
        throw exceptions::OpensslError("DH_get0_p()");
    }
    std::size_t param_q_data_length = BN_bn2mpi(param_q_bn, nullptr);
    unsigned char param_q_data[param_q_data_length];
    BN_bn2mpi(param_q_bn, param_q_data);
    param_q_ = std::vector<unsigned char>(param_q_data,
                                          param_q_data + param_q_data_length);

    // param g
    const BIGNUM* param_g_bn = DH_get0_g(params_dh);
    if (param_g_bn == nullptr) {
        throw exceptions::OpensslError("DH_get0_p()");
    }
    std::size_t param_g_data_length = BN_bn2mpi(param_g_bn, nullptr);
    unsigned char param_g_data[param_g_data_length];
    BN_bn2mpi(param_g_bn, param_g_data);
    param_g_ = std::vector<unsigned char>(param_g_data,
                                          param_g_data + param_g_data_length);
}

DiffieHellman::DiffieHellman() : impl_(std::make_unique<Impl>()) {}

DiffieHellman::~DiffieHellman() = default;

void DiffieHellman::peer_public_key(
    const std::vector<unsigned char>& peer_public_key) {
    impl_->peer_public_key(peer_public_key);
}

void DiffieHellman::peer_public_key(
    const std::vector<unsigned char>& peer_public_key,
    const std::size_t& peer_public_key_length) {
    impl_->peer_public_key(peer_public_key, peer_public_key_length);
}

void DiffieHellman::peer_public_key(
    std::vector<unsigned char>&& peer_public_key) {
    impl_->peer_public_key(std::move(peer_public_key));
}

void DiffieHellman::peer_public_key(
    std::vector<unsigned char>&& peer_public_key,
    const std::size_t& peer_public_key_length) {
    impl_->peer_public_key(std::move(peer_public_key), peer_public_key_length);
}

void DiffieHellman::peer_public_key(const char* peer_public_key) {
    impl_->peer_public_key(peer_public_key);
}

void DiffieHellman::peer_public_key(const char* peer_public_key,
                                    const std::size_t& peer_public_key_length) {
    impl_->peer_public_key(peer_public_key, peer_public_key_length);
}

void DiffieHellman::peer_public_key(const unsigned char* peer_public_key) {
    impl_->peer_public_key(peer_public_key);
}

void DiffieHellman::peer_public_key(const unsigned char* peer_public_key,
                                    const std::size_t& peer_public_key_length) {
    impl_->peer_public_key(peer_public_key, peer_public_key_length);
}

std::vector<unsigned char>& DiffieHellman::peer_public_key() {
    return impl_->peer_public_key();
}

const std::vector<unsigned char>& DiffieHellman::peer_public_key() const {
    return impl_->peer_public_key();
}

void DiffieHellman::param_p(const std::vector<unsigned char>& param_p) {
    impl_->param_p(param_p);
}

void DiffieHellman::param_p(const std::vector<unsigned char>& param_p,
                            const std::size_t& param_p_length) {
    impl_->param_p(param_p, param_p_length);
}

void DiffieHellman::param_p(std::vector<unsigned char>&& param_p) {
    impl_->param_p(std::move(param_p));
}

void DiffieHellman::param_p(std::vector<unsigned char>&& param_p,
                            const std::size_t& param_p_length) {
    impl_->param_p(std::move(param_p), param_p_length);
}

void DiffieHellman::param_p(const char* param_p) {
    impl_->param_p(param_p);
}

void DiffieHellman::param_p(const char* param_p,
                            const std::size_t& param_p_length) {
    impl_->param_p(param_p, param_p_length);
}

void DiffieHellman::param_p(const unsigned char* param_p) {
    impl_->param_p(param_p);
}

void DiffieHellman::param_p(const unsigned char* param_p,
                            const std::size_t& param_p_length) {
    impl_->param_p(param_p, param_p_length);
}

std::vector<unsigned char>& DiffieHellman::param_p() {
    return impl_->param_p();
}

const std::vector<unsigned char>& DiffieHellman::param_p() const {
    return impl_->param_p();
}

void DiffieHellman::param_q(const std::vector<unsigned char>& param_q) {
    impl_->param_q(param_q);
}

void DiffieHellman::param_q(const std::vector<unsigned char>& param_q,
                            const std::size_t& param_q_length) {
    impl_->param_q(param_q, param_q_length);
}

void DiffieHellman::param_q(std::vector<unsigned char>&& param_q) {
    impl_->param_q(std::move(param_q));
}

void DiffieHellman::param_q(std::vector<unsigned char>&& param_q,
                            const std::size_t& param_q_length) {
    impl_->param_q(std::move(param_q), param_q_length);
}

void DiffieHellman::param_q(const char* param_q) {
    impl_->param_q(param_q);
}

void DiffieHellman::param_q(const char* param_q,
                            const std::size_t& param_q_length) {
    impl_->param_q(param_q, param_q_length);
}

void DiffieHellman::param_q(const unsigned char* param_q) {
    impl_->param_q(param_q);
}

void DiffieHellman::param_q(const unsigned char* param_q,
                            const std::size_t& param_q_length) {
    impl_->param_q(param_q, param_q_length);
}

std::vector<unsigned char>& DiffieHellman::param_q() {
    return impl_->param_q();
}

const std::vector<unsigned char>& DiffieHellman::param_q() const {
    return impl_->param_q();
}

void DiffieHellman::param_g(const std::vector<unsigned char>& param_g) {
    impl_->param_g(param_g);
}

void DiffieHellman::param_g(const std::vector<unsigned char>& param_g,
                            const std::size_t& param_g_length) {
    impl_->param_g(param_g, param_g_length);
}

void DiffieHellman::param_g(std::vector<unsigned char>&& param_g) {
    impl_->param_g(std::move(param_g));
}

void DiffieHellman::param_g(std::vector<unsigned char>&& param_g,
                            const std::size_t& param_g_length) {
    impl_->param_g(std::move(param_g), param_g_length);
}

void DiffieHellman::param_g(const char* param_g) {
    impl_->param_g(param_g);
}

void DiffieHellman::param_g(const char* param_g,
                            const std::size_t& param_g_length) {
    impl_->param_g(param_g, param_g_length);
}

void DiffieHellman::param_g(const unsigned char* param_g) {
    impl_->param_g(param_g);
}

void DiffieHellman::param_g(const unsigned char* param_g,
                            const std::size_t& param_g_length) {
    impl_->param_g(param_g, param_g_length);
}

std::vector<unsigned char>& DiffieHellman::param_g() {
    return impl_->param_g();
}

const std::vector<unsigned char>& DiffieHellman::param_g() const {
    return impl_->param_g();
}

const std::vector<unsigned char>& DiffieHellman::public_key() const {
    return impl_->public_key();
}

const std::vector<unsigned char>& DiffieHellman::private_key() const {
    return impl_->private_key();
}

const std::vector<unsigned char>& DiffieHellman::shared_key() const {
    return impl_->shared_key();
}

void DiffieHellman::use_built_in_params(const bool& built_in) {
    impl_->use_built_in_params(built_in);
}

void DiffieHellman::use_built_in_params(bool&& built_in) {
    impl_->use_built_in_params(std::move(built_in));
}

const bool& DiffieHellman::use_built_in_params() const {
    return impl_->use_built_in_params();
}

bool& DiffieHellman::use_built_in_params() {
    return impl_->use_built_in_params();
}

void DiffieHellman::generate_key_pair() {
    impl_->generate_key_pair();
}

void DiffieHellman::derive_shared_key() {
    impl_->derive_shared_key();
}

} // namespace crypto
} // namespace utils
} // namespace rayalto
