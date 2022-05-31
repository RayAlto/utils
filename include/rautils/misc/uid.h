#ifndef RA_UTILS_RAUTILS_MISC_UID_H_
#define RA_UTILS_RAUTILS_MISC_UID_H_

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>

namespace rayalto {
namespace utils {
namespace misc {

/**
 * Unique identifier (inspired by IPv6 address)
 */
class Uid {
public:
    static const Uid INVALID;

public:
    Uid(const std::uint16_t& part1,
        const std::uint16_t& part2,
        const std::uint16_t& part3,
        const std::uint16_t& part4,
        const std::uint16_t& part5,
        const std::uint16_t& part6,
        const std::uint16_t& part7,
        const std::uint16_t& part8);
    Uid(const std::array<std::uint16_t, 8>& parts);
    Uid(std::array<std::uint16_t, 8>&& parts);
    Uid(const std::string& uid);

    Uid() = default;
    Uid(const Uid&) = default;
    Uid(Uid&&) noexcept = default;
    Uid& operator=(const Uid&) = default;
    Uid& operator=(Uid&&) noexcept = default;

    virtual ~Uid() = default;

public:
    Uid& operator++();
    Uid operator++(int);
    Uid& operator--();
    Uid operator--(int);
    Uid& operator+=(const Uid& rv);
    Uid& operator+=(const std::uint16_t& rv);
    Uid& operator-=(const Uid& rv);
    Uid& operator-=(const std::uint16_t& rv);
    friend Uid operator+(Uid lv, const Uid& rv);
    friend Uid operator+(Uid lv, const std::uint16_t& rv);
    friend Uid operator-(Uid lv, const Uid& rv);
    friend Uid operator-(Uid lv, const std::uint16_t& rv);
    friend bool operator<(const Uid& lv, const Uid& rv);
    friend bool operator>(const Uid& lv, const Uid& rv);
    friend bool operator<=(const Uid& lv, const Uid& rv);
    friend bool operator>=(const Uid& lv, const Uid& rv);
    friend bool operator==(const Uid& lv, const Uid& rv);
    friend bool operator!=(const Uid& lv, const Uid& rv);
    const std::uint16_t& operator[](std::size_t index) const;
    std::uint16_t& operator[](std::size_t index);

public:
    std::string to_string(const bool& simplify = true,
                          const char& padding = '\0') const;

protected:
    std::array<std::uint16_t, 8> parts_;

protected:
    void check_carry(std::size_t cursor);
    void check_borrow(std::size_t cursor);
};

} // namespace misc
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_MISC_UID_H_
