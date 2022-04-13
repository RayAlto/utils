#ifndef RA_UTILS_CRYPTO_DIFFIE_HELLMAN_H_
#define RA_UTILS_CRYPTO_DIFFIE_HELLMAN_H_

#include <memory>

namespace rayalto {
namespace utils {
namespace crypto {

class DiffieHellman {
public:
    DiffieHellman() = default;
    DiffieHellman(const DiffieHellman&) = delete;
    DiffieHellman(DiffieHellman&&) noexcept = default;
    DiffieHellman& operator=(const DiffieHellman&) = delete;
    DiffieHellman& operator=(DiffieHellman&&) noexcept = default;

    virtual ~DiffieHellman();

protected:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace crypto
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_CRYPTO_DIFFIE_HELLMAN_H_
