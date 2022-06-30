#ifndef RA_UTILS_RAUTILS_BARCODE_QRCODE_H_
#define RA_UTILS_RAUTILS_BARCODE_QRCODE_H_

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <vector>

namespace rayalto {
namespace utils {
namespace barcode {

class Qrcode {
public:
    class Result;

    // error correction level
    enum class EC : std::uint8_t {
        L, /* Low, 7% */
        M, /* Medium, 15% */
        Q, /* Quality, 25% */
        H /* High, 30% */
    };
    // mask
    enum class Mask : std::uint8_t { M0, M1, M2, M3, M4, M5, M6, M7, AUTO };

public:
    Qrcode() = default;
    Qrcode(const Qrcode&) = default;
    Qrcode(Qrcode&&) noexcept = default;
    Qrcode& operator=(const Qrcode&) = default;
    Qrcode& operator=(Qrcode&&) noexcept = default;

    virtual ~Qrcode() = default;

    const std::uint8_t& margin() const;
    std::uint8_t& margin();
    Qrcode& margin(const std::uint8_t& margin);
    Qrcode& margin(std::uint8_t&& margin);

    const std::uint8_t& version() const;
    std::uint8_t& version();
    Qrcode& version(const std::uint8_t& version);
    Qrcode& version(std::uint8_t&& version);

    const EC& error_correction_level() const;
    EC& error_correction_level();
    Qrcode& error_correction_level(const EC& ec);
    Qrcode& error_correction_level(EC&& ec);

    const Mask& mask() const;
    Mask& mask();
    Qrcode& mask(const Mask& mask);
    Qrcode& mask(Mask&& mask);

    Result encode(const std::string& text);
    Result encode(const std::vector<unsigned char>& binary);

protected:
    std::uint8_t margin_ = 4;
    std::uint8_t version_ = 0;
    EC ec_ = EC::M;
    Mask mask_ = Mask::AUTO;
};

class Qrcode::Result {
public:
    Result() = default;
    Result(const Result&) = default;
    Result(Result&&) noexcept = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&&) noexcept = default;

    virtual ~Result() = default;

    Result(const std::size_t& width, const std::size_t& height);
    Result(std::size_t&& width, std::size_t&& height);

    Result(const std::size_t& width,
           const std::size_t& height,
           const std::vector<bool>& data);
    Result(std::size_t&& width, std::size_t&& height, std::vector<bool>&& data);

    Result& reverse_color_in_ostream(const bool& reverse);
    Result& reverse_color_in_ostream(bool&& reverse);
    const bool& reverse_color_in_ostream() const;
    bool& reverse_color_in_ostream();
    friend std::ostream& operator<<(std::ostream& os, const Result& result);
    std::string to_svg(const std::size_t& rect_size = 16) const;
    bool get(const std::size_t& x, const std::size_t& y) const;
    Result& set(const std::size_t& x,
                const std::size_t& y,
                const bool& value = true);

    bool empty() const noexcept;
    Result& reset() noexcept;
    Result& resize(const std::size_t& width, const std::size_t& height);
    Result& resize(std::size_t&& width, std::size_t&& height);
    Result& resize(const std::size_t& width,
                   const std::size_t& height,
                   const bool& value);
    Result& resize(std::size_t&& width,
                   std::size_t&& height,
                   const bool& value);
    Result& flip();
    friend bool operator==(const Result& lhs, const Result& rhs);

protected:
    std::vector<bool> data_;
    std::size_t width_ = 0;
    std::size_t height_ = 0;
    bool reverse_in_ostream_ = false;
};

} // namespace barcode
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_BARCODE_QRCODE_H_
