#ifndef RA_UTILS_WEBSOCKET_CLOSE_STATUS_H_
#define RA_UTILS_WEBSOCKET_CLOSE_STATUS_H_

#include <cstdint>
#include <string>

namespace rayalto {
namespace utils {
namespace websocket {

struct CloseStatus {
    // https://developer.mozilla.org/zh-CN/docs/Web/API/CloseEvent
    // https://github.com/Luka967/websocket-close-codes
    enum class Code : std::uint16_t {
        UNKNOWN,
        NORMAL = 1000,
        GOING_AWAY,
        PROTOCOL_ERROR,
        UNSUPPORTED,
        RESERVED,
        NO_STATUS,
        ABNORMAL,
        UNSUPPORTED_DATA,
        POLICY_VIOLATION,
        TOO_LARGE,
        MISSING_EXTENSION,
        INTERNAL_ERROR,
        SERVICE_RESTART,
        TRY_AGAIN_LATER,
        BAD_GATEWAY,
        TLS_HANDSHAKE
    };
    static constexpr Code code_of(const std::uint16_t& code);
    static constexpr std::uint16_t value_of(const Code& code);

    std::uint16_t code;
    std::string status;
    std::string reason;

    CloseStatus(std::uint16_t close_code) : code(close_code) {}
    CloseStatus(std::uint16_t close_code, std::string close_reason) :
        code(close_code), reason(close_reason) {}
    CloseStatus(std::uint16_t close_code,
                std::string close_status,
                std::string close_reason) :
        code(close_code), status(close_status), reason(close_reason) {}

    CloseStatus(CloseStatus::Code close_code) :
        CloseStatus(static_cast<std::uint16_t>(close_code)) {}
    CloseStatus(CloseStatus::Code close_code, const std::string close_reason) :
        CloseStatus(static_cast<std::uint16_t>(close_code), close_reason) {}
    CloseStatus(CloseStatus::Code close_code,
                std::string close_status,
                std::string close_reason) :
        CloseStatus(static_cast<std::uint16_t>(close_code),
                    close_status,
                    close_reason) {}
};

} // namespace websocket
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_WEBSOCKET_CLOSE_STATUS_H_
