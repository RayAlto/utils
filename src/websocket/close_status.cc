#include "websocket/close_status.h"

namespace rayalto {
namespace utils {
namespace websocket {
namespace close_status {

constexpr Code code_of(const std::uint16_t& code) {
    if (code < 1000 || code > 1015) {
        return Code::UNKNOWN;
    }
    return static_cast<Code>(code);
}

constexpr std::uint16_t value_of(const Code& code) {
    return static_cast<std::uint16_t>(code);
}

} // namespace close_status
} // namespace websocket
} // namespace utils
} // namespace rayalto
