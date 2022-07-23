#include "rautils/barcode/qrcode.h"

#include <cstddef>
#include <cstdint>
#include <utility>

#include "ZXing/BarcodeFormat.h"
#include "ZXing/BitMatrix.h"
#include "ZXing/CharacterSet.h"
#include "ZXing/MultiFormatWriter.h"

namespace rayalto::utils::barcode {

namespace {

void prepare_zxing_qr_writer(ZXing::MultiFormatWriter& writer,
                             const std::uint8_t& margin,
                             const Qrcode::EC& ec,
                             const ZXing::CharacterSet& character_set) {
    writer.setMargin(margin);
    writer.setEccLevel(static_cast<int>(ec));
    writer.setEncoding(character_set);
}

Qrcode::Result convert_to_qrcode_result(const ZXing::BitMatrix& qr_matrix) {
    const int result_width = qr_matrix.width();
    const int result_height = qr_matrix.height();

    Qrcode::Result result(result_width, result_height);
    for (int y = 0; y < result_height; ++y) {
        for (int x = 0; x < result_width; ++x) {
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

Qrcode::Result Qrcode::encode(const std::string& text) {
    ZXing::MultiFormatWriter qr_writer(ZXing::BarcodeFormat::QRCode);
    prepare_zxing_qr_writer(qr_writer, margin_, ec_, ZXing::CharacterSet::UTF8);
    ZXing::BitMatrix qr_matrix = qr_writer.encode(text, 1, 1);
    return convert_to_qrcode_result(qr_matrix);
}

Qrcode::Result Qrcode::encode(const std::vector<unsigned char>& binary) {
    ZXing::MultiFormatWriter qr_writer(ZXing::BarcodeFormat::QRCode);
    prepare_zxing_qr_writer(
        qr_writer, margin_, ec_, ZXing::CharacterSet::BINARY);
    ZXing::BitMatrix qr_matrix =
        qr_writer.encode(std::wstring(binary.begin(), binary.end()), 1, 1);
    return convert_to_qrcode_result(qr_matrix);
}

} // namespace rayalto::utils::barcode
