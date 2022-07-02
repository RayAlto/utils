#ifndef RA_UTILS_CRYPTO_DIFFIE_HELLMAN_H_
#define RA_UTILS_CRYPTO_DIFFIE_HELLMAN_H_

#include <memory>
#include <vector>

namespace rayalto::utils::crypto {

/**
 * A simple Diffie-Hellman Key Exchange (ANSI X9.42) implement
 *
 * Example:
 *   Alice:
 *
 *     DiffieHellman alice;
 *     alice.generate_key_pair(); // generate params and key pair
 *     alice.public_key();        // get generated public key
 *     alice.param_p();           // get generated param p
 *     alice.param_q();           // get generated param q
 *     alice.param_g();           // get generated param g
 *
 *   Bob:
 *
 *     DiffieHellman bob;
 *     bob.param_p(alice_param_p);            // set param p
 *     bob.param_p(alice_param_q);            // set param q
 *     bob.param_p(alice_param_g);            // set param g
 *     bob.generate_key_pair();               // generate key pair
 *     bob.peer_public_key(alice_public_key); // set peer public key
 *     bob.derive_shared_key();               // derive shared key
 *     bob.shared_key();                      // get derived shared key
 *
 *   Alice:
 *
 *     alice.peer_public_key(bob_public_key); // set peer public key
 *     alice.derive_shared_key();             // derive shared key
 *     alice.shared_key();                    // get derived shared key
 *
 * Note:
 *   Key generation parameters (before DiffieHellman::generate_key_pair() called):
 *     1. DiffieHellman::use_built_in_params(true) -> use OpenSSL built-in params
 *     2. param_p/q/g(param) -> use custom params
 *     3. generate params
 */
class DiffieHellman {
public:
    DiffieHellman();
    DiffieHellman(const DiffieHellman&) = delete;
    DiffieHellman(DiffieHellman&&) noexcept = default;
    DiffieHellman& operator=(const DiffieHellman&) = delete;
    DiffieHellman& operator=(DiffieHellman&&) noexcept = default;

    virtual ~DiffieHellman();

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
    [[nodiscard]] const std::vector<unsigned char>& peer_public_key() const;

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
    [[nodiscard]] const std::vector<unsigned char>& param_p() const;

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
    [[nodiscard]] const std::vector<unsigned char>& param_q() const;

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
    [[nodiscard]] const std::vector<unsigned char>& param_g() const;

    // generated public key
    [[nodiscard]] const std::vector<unsigned char>& public_key() const;
    // generated private key
    [[nodiscard]] const std::vector<unsigned char>& private_key() const;
    // generated shared key
    [[nodiscard]] const std::vector<unsigned char>& shared_key() const;

    // force use OpenSSL built-in generation parameters
    void use_built_in_params(const bool& built_in);
    [[nodiscard]] const bool& use_built_in_params() const;
    bool& use_built_in_params();

    // generate key pair using generation parameters
    void generate_key_pair();

    // derive shared key with peer public key
    void derive_shared_key();

protected:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace rayalto::utils::crypto

#endif // RA_UTILS_CRYPTO_DIFFIE_HELLMAN_H_
