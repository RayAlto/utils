#ifndef RA_UTILS_RAUTILS_BARCODE_QRCODE_H_
#define RA_UTILS_RAUTILS_BARCODE_QRCODE_H_

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <vector>

namespace rayalto::utils::barcode {

class Qrcode {
public:
    class Result;

    // error correction level
    enum class EC : std::uint8_t {
        L = 1, /* Low, 7% */
        M = 3, /* Medium, 15% */
        Q = 5, /* Quality, 25% */
        H = 7 /* High, 30% */
    };

    Qrcode() = default;
    Qrcode(const Qrcode&) = default;
    Qrcode(Qrcode&&) noexcept = default;
    Qrcode& operator=(const Qrcode&) = default;
    Qrcode& operator=(Qrcode&&) noexcept = default;

    virtual ~Qrcode() = default;

    // margin of qr code
    [[nodiscard]] const std::uint8_t& margin() const;
    std::uint8_t& margin();
    Qrcode& margin(const std::uint8_t& margin);

    // error correction level
    [[nodiscard]] const EC& error_correction_level() const;
    EC& error_correction_level();
    Qrcode& error_correction_level(const EC& ec);

    // encode utf-8 text
    Result encode(const std::string& text);

    // encode binary data
    Result encode(const std::vector<unsigned char>& binary);

protected:
    std::uint8_t margin_ = 4;
    EC ec_ = EC::M;
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

    Result(const std::size_t& width,
           const std::size_t& height,
           const std::vector<bool>& data);
    Result(const std::size_t& width,
           const std::size_t& height,
           std::vector<bool>&& data);

    Result& reverse_color_in_ostream(const bool& reverse);
    [[nodiscard]] const bool& reverse_color_in_ostream() const;
    bool& reverse_color_in_ostream();

    friend std::ostream& operator<<(std::ostream& os, const Result& result);

    [[nodiscard]] std::string to_svg(const std::size_t& rect_size = 16) const;

    [[nodiscard]] bool get(const std::size_t& x, const std::size_t& y) const;

    Result& set(const std::size_t& x,
                const std::size_t& y,
                const bool& value = true);

    [[nodiscard]] bool empty() const noexcept;

    Result& reset() noexcept;

    Result& resize(const std::size_t& width, const std::size_t& height);
    Result& resize(const std::size_t& width,
                   const std::size_t& height,
                   const bool& value);

    Result& flip();

    friend bool operator==(const Result& lhs, const Result& rhs);

protected:
    std::vector<bool> data_;
    std::size_t width_ = 0;
    std::size_t height_ = 0;
    bool reverse_in_ostream_ = false;
};

} // namespace rayalto::utils::barcode

#endif // RA_UTILS_RAUTILS_BARCODE_QRCODE_H_
