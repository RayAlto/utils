#ifndef RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_CLOSE_STATUS_H_
#define RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_CLOSE_STATUS_H_

#include <cstdint>

namespace rayalto {
namespace utils {
namespace network {
namespace websocket {

class CloseStatus {
public:
    // https://developer.mozilla.org/en-US/docs/Web/API/CloseEvent/code
    enum Code : std::uint16_t {
        /* 1000 */ NORMAL = 1000,
        /* 1001 */ GOING_AWAY,
        /* 1002 */ PROTOCOL_ERROR,
        /* 1003 */ UNSUPPORTED,
        /* 1005 */ NO_STATUS = 1005,
        /* 1006 */ ABNORMAL,
        /* 1007 */ UNSUPPORTED_DATA,
        /* 1008 */ POLICY_VIOLATION,
        /* 1009 */ TOO_LARGE,
        /* 1010 */ EXTENSION_REQUIRED,
        /* 1011 */ INTERNAL_ERROR,
        /* 1015 */ TLS_FAILURE = 1015
    };

public:
    CloseStatus() = default;
    CloseStatus(const CloseStatus&) = default;
    CloseStatus(CloseStatus&&) noexcept = default;
    CloseStatus& operator=(const CloseStatus&) = default;
    CloseStatus& operator=(CloseStatus&&) noexcept = default;

    CloseStatus(const Code& code);
    CloseStatus(Code&& code);

    CloseStatus(const std::uint16_t& value);
    CloseStatus(std::uint16_t&& value);

    const std::uint16_t& value() const;
    std::uint16_t& value();

    bool is_unknown() const;

    friend bool operator==(const CloseStatus& lv, const CloseStatus& rv);
    friend bool operator!=(const CloseStatus& lv, const CloseStatus& rv);

    virtual ~CloseStatus() = default;

protected:
    std::uint16_t value_ = 0xffff;
};

} // namespace websocket
} // namespace network
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_CLOSE_STATUS_H_
