#ifndef RA_UTILS_RAUTILS_MISC_UID_H_
#define RA_UTILS_RAUTILS_MISC_UID_H_

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>

namespace rayalto::utils::misc {

/**
 * Unique identifier (inspired by IPv6 address)
 */
class Uid {
public:
    static const Uid INVALID;

    Uid(const std::uint16_t& part1,
        const std::uint16_t& part2,
        const std::uint16_t& part3,
        const std::uint16_t& part4,
        const std::uint16_t& part5,
        const std::uint16_t& part6,
        const std::uint16_t& part7,
        const std::uint16_t& part8);
    explicit Uid(const std::array<std::uint16_t, 8>& parts);
    explicit Uid(const std::string& uid);

    Uid() = default;
    Uid(const Uid&) = default;
    Uid(Uid&&) noexcept = default;
    Uid& operator=(const Uid&) = default;
    Uid& operator=(Uid&&) noexcept = default;

    virtual ~Uid() = default;

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

    [[nodiscard]] std::string to_string(const bool& simplify = true,
                                        const char& padding = '\0') const;

protected:
    std::array<std::uint16_t, 8> parts_ {};

    void check_carry(std::size_t cursor);
    void check_borrow(std::size_t cursor);
};

} // namespace rayalto::utils::misc

#endif // RA_UTILS_RAUTILS_MISC_UID_H_
