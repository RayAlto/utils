#include "rautils/network/websocket/close_status.h"

#include <cstdint>
#include <utility>

namespace rayalto {
namespace utils {
namespace network {
namespace websocket {

CloseStatus::CloseStatus(const Code& code) :
    value_(static_cast<std::uint16_t>(code)) {}

CloseStatus::CloseStatus(Code&& code) :
    value_(static_cast<std::uint16_t>(std::move(code))) {}

CloseStatus::CloseStatus(const std::uint16_t& value) : value_(value) {}

CloseStatus::CloseStatus(std::uint16_t&& value) : value_(std::move(value)) {}

const std::uint16_t& CloseStatus::value() const {
    return value_;
}

std::uint16_t& CloseStatus::value() {
    return value_;
}

bool CloseStatus::is_unknown() const {
    switch (value_) {
    case 1000:
    case 1001:
    case 1002:
    case 1003:
    case 1005:
    case 1006:
    case 1007:
    case 1008:
    case 1009:
    case 1010:
    case 1011:
    case 1015: return false; break;
    default: return true; break;
    }
    return true;
}

bool operator==(const CloseStatus& lv, const CloseStatus& rv) {
    return lv.value_ == rv.value_;
}

bool operator!=(const CloseStatus& lv, const CloseStatus& rv) {
    return !operator==(lv, rv);
}

} // namespace websocket
} // namespace network
} // namespace utils
} // namespace rayalto
