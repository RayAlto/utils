#include "rautils/network/ws_client.h"

namespace rayalto {
namespace utils {
namespace network {

constexpr WsClient::CloseStatus::Code WsClient::CloseStatus::code_of(
    const std::uint16_t& code) {
    if (code < 1000 || code > 1015) {
        return Code::UNKNOWN;
    }
    return static_cast<Code>(code);
}

constexpr std::uint16_t WsClient::CloseStatus::value_of(const Code& code) {
    return static_cast<std::uint16_t>(code);
}

} // namespace network
} // namespace utils
} // namespace rayalto
