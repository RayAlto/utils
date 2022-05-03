#include "websocket/close_status.h"

namespace rayalto {
namespace utils {
namespace websocket {

constexpr CloseStatus::Code CloseStatus::code_of(const std::uint16_t& code) {
    if (code < 1000 || code > 1015) {
        return Code::UNKNOWN;
    }
    return static_cast<Code>(code);
}

constexpr std::uint16_t CloseStatus::value_of(const Code& code) {
    return static_cast<std::uint16_t>(code);
}

} // namespace websocket
} // namespace utils
} // namespace rayalto
