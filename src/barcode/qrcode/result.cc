#include "rautils/barcode/qrcode.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace rayalto::utils::barcode {

Qrcode::Result::Result(const std::size_t& width, const std::size_t& height) :
    data_(width * height, false), width_(width), height_(height) {}

Qrcode::Result::Result(const std::size_t& width,
                       const std::size_t& height,
                       const std::vector<bool>& data) :
    data_(data), width_(width), height_(height) {}

Qrcode::Result::Result(const std::size_t& width,
                       const std::size_t& height,
                       std::vector<bool>&& data) :
    data_(std::move(data)), width_(width), height_(height) {}

Qrcode::Result& Qrcode::Result::reverse_color_in_ostream(const bool& reverse) {
    reverse_in_ostream_ = reverse;
    return *this;
}

const bool& Qrcode::Result::reverse_color_in_ostream() const {
    return reverse_in_ostream_;
}

bool& Qrcode::Result::reverse_color_in_ostream() {
    return reverse_in_ostream_;
}

std::ostream& operator<<(std::ostream& os, const Qrcode::Result& result) {
    if (result.reverse_in_ostream_) {
        for (std::size_t y = 0; y < result.height_; ++y) {
            for (std::size_t x = 0; x < result.width_; ++x) {
                if (result.data_[y * result.width_ + x]) {
                    os << "  ";
                }
                else {
                    os << "██";
                }
            }
            os << '\n';
        }
    }
    else {
        for (std::size_t y = 0; y < result.height_; ++y) {
            for (std::size_t x = 0; x < result.width_; ++x) {
                if (result.data_[y * result.width_ + x]) {
                    os << "██";
                }
                else {
                    os << "  ";
                }
            }
            os << '\n';
        }
    }
    return os;
}

std::string Qrcode::Result::to_svg(const std::size_t& rect_size) const {
    std::size_t real_width = width_ * rect_size;
    std::size_t real_height = height_ * rect_size;

    const std::string rect_size_str = std::to_string(rect_size);
    const std::string real_width_str = std::to_string(real_width);
    const std::string real_height_str = std::to_string(real_height);

    std::string result = R"+*(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width=")+*";
    result += real_width_str;
    result += R"+*(" height=")+*";
    result += real_height_str;
    result += R"+*(" viewBox="0 0 )+*";
    result += real_width_str;
    result += ' ';
    result += real_height_str;
    result += R"+*("><g id="qr"><rect x="0" y="0" width=")+*";
    result += real_width_str;
    result += R"+*(" height=")+*";
    result += real_height_str;
    result += R"+*(" fill="#ffffff"/><g id="pattern" fill="#000000">)+*";

    std::string rect_end = R"+*(" width=")+*";
    rect_end += rect_size_str;
    rect_end += R"+*(" height=")+*";
    rect_end += rect_size_str;
    rect_end += R"+*("/>)+*";

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            if (data_[y * width_ + x]) {
                result += R"+*(<rect x=")+*";
                result += std::to_string(x * rect_size);
                result += R"+*(" y=")+*";
                result += std::to_string(y * rect_size);
                result += rect_end;
            }
        }
    }

    result += R"+*(</g></g></svg>)+*";

    return result;
}

bool Qrcode::Result::get(const std::size_t& x, const std::size_t& y) const {
    return data_.at(width_ * y + x);
}

Qrcode::Result& Qrcode::Result::set(const std::size_t& x,
                                    const std::size_t& y,
                                    const bool& value) {
    data_.at(width_ * y + x) = value;
    return *this;
}

bool Qrcode::Result::empty() const noexcept {
    return data_.empty();
}

Qrcode::Result& Qrcode::Result::reset() noexcept {
    std::fill(data_.begin(), data_.end(), false);
    return *this;
}

Qrcode::Result& Qrcode::Result::resize(const std::size_t& width,
                                       const std::size_t& height) {
    data_ = std::vector<bool>(width * height, false);
    width_ = width;
    height_ = height;
    return *this;
}

Qrcode::Result& Qrcode::Result::resize(const std::size_t& width,
                                       const std::size_t& height,
                                       const bool& value) {
    data_ = std::vector<bool>(width * height, value);
    width_ = width;
    height_ = height;
    return *this;
}

Qrcode::Result& Qrcode::Result::flip() {
    data_.flip();
    return *this;
}

bool operator==(const Qrcode::Result& lhs, const Qrcode::Result& rhs) {
    return lhs.data_ == rhs.data_;
}

} // namespace rayalto::utils::barcode
