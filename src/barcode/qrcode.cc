#include "rautils/barcode/qrcode.h"

#include <cstddef>
#include <cstdint>
#include <utility>

#include "ZXing/BitMatrix.h"
#include "ZXing/CharacterSet.h"
#include "ZXing/qrcode/QRErrorCorrectionLevel.h"
#include "ZXing/qrcode/QRWriter.h"
#include "ZXing/TextUtfEncoding.h"

namespace rayalto {
namespace utils {
namespace barcode {

namespace {

void prepare_zxing_qr_writer(ZXing::QRCode::Writer& writer,
                             const std::uint8_t& margin,
                             const std::uint8_t& version,
                             const Qrcode::EC& ec,
                             const Qrcode::Mask& mask) {
    if (margin != 4) {
        writer.setMargin(margin);
    }
    if (version != 0) {
        writer.setVersion(version);
    }
    if (ec != Qrcode::EC::L) {
        writer.setErrorCorrectionLevel(
            static_cast<ZXing::QRCode::ErrorCorrectionLevel>(ec));
    }
    if (mask != Qrcode::Mask::AUTO) {
        writer.setMaskPattern(static_cast<int>(mask));
    }
}

Qrcode::Result convert_to_qrcode_result(const ZXing::BitMatrix& qr_matrix) {
    const std::size_t result_width = qr_matrix.width();
    const std::size_t result_height = qr_matrix.height();

    Qrcode::Result result(result_width, result_height);
    for (std::size_t y = 0; y < result_height; ++y) {
        for (std::size_t x = 0; x < result_width; ++x) {
            if (qr_matrix.get(x, y)) {
                result.set(x, y);
            }
        }
    }

    return result;
}

} // anonymous namespace

const std::uint8_t& Qrcode::margin() const {
    return margin_;
}

std::uint8_t& Qrcode::margin() {
    return margin_;
}

Qrcode& Qrcode::margin(const std::uint8_t& margin) {
    margin_ = margin;
    return *this;
}

Qrcode& Qrcode::margin(std::uint8_t&& margin) {
    margin_ = std::move(margin);
    return *this;
}

const std::uint8_t& Qrcode::version() const {
    return version_;
}

std::uint8_t& Qrcode::version() {
    return version_;
}

Qrcode& Qrcode::version(const std::uint8_t& version) {
    version_ = version;
    return *this;
}

Qrcode& Qrcode::version(std::uint8_t&& version) {
    version_ = std::move(version);
    return *this;
}

const Qrcode::EC& Qrcode::error_correction_level() const {
    return ec_;
}

Qrcode::EC& Qrcode::error_correction_level() {
    return ec_;
}

Qrcode& Qrcode::error_correction_level(const EC& ec) {
    ec_ = ec;
    return *this;
}

Qrcode& Qrcode::error_correction_level(EC&& ec) {
    ec_ = std::move(ec);
    return *this;
}

const Qrcode::Mask& Qrcode::mask() const {
    return mask_;
}

Qrcode::Mask& Qrcode::mask() {
    return mask_;
}

Qrcode& Qrcode::mask(const Qrcode::Mask& mask) {
    mask_ = mask;
    return *this;
}

Qrcode& Qrcode::mask(Qrcode::Mask&& mask) {
    mask_ = std::move(mask);
    return *this;
}

Qrcode::Result Qrcode::encode(const std::string& text) {
    ZXing::QRCode::Writer qr_writer;
    prepare_zxing_qr_writer(qr_writer, margin_, version_, ec_, mask_);
    qr_writer.setEncoding(ZXing::CharacterSet::UTF8);
    ZXing::BitMatrix qr_matrix =
        qr_writer.encode(ZXing::TextUtfEncoding::FromUtf8(text), 1, 1);
    return convert_to_qrcode_result(qr_matrix);
}

Qrcode::Result Qrcode::encode(const std::vector<unsigned char>& binary) {
    ZXing::QRCode::Writer qr_writer;
    prepare_zxing_qr_writer(qr_writer, margin_, version_, ec_, mask_);
    qr_writer.setEncoding(ZXing::CharacterSet::BINARY);
    ZXing::BitMatrix qr_matrix =
        qr_writer.encode(std::wstring(binary.begin(), binary.end()), 1, 1);
    return convert_to_qrcode_result(qr_matrix);
}

} // namespace barcode
} // namespace utils
} // namespace rayalto
