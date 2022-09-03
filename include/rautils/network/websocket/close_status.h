#ifndef RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_CLOSE_STATUS_H_
#define RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_CLOSE_STATUS_H_

#include <cstdint>

namespace rayalto::utils::network::websocket {

class CloseStatus {
public:
    // https://developer.mozilla.org/en-US/docs/Web/API/CloseEvent/code
    /* clang-format off */
    enum Code : std::uint16_t {
        /* 1000 */ NORMAL             = 1000,
        /* 1001 */ GOING_AWAY               ,
        /* 1002 */ PROTOCOL_ERROR           ,
        /* 1003 */ UNSUPPORTED              ,
        /* 1005 */ NO_STATUS          = 1005,
        /* 1006 */ ABNORMAL                 ,
        /* 1007 */ UNSUPPORTED_DATA         ,
        /* 1008 */ POLICY_VIOLATION         ,
        /* 1009 */ TOO_LARGE                ,
        /* 1010 */ EXTENSION_REQUIRED       ,
        /* 1011 */ INTERNAL_ERROR           ,
        /* 1015 */ TLS_FAILURE        = 1015,
    };
    /* clang-format on */

    CloseStatus() = default;
    CloseStatus(const CloseStatus&) = default;
    CloseStatus(CloseStatus&&) noexcept = default;
    CloseStatus& operator=(const CloseStatus&) = default;
    CloseStatus& operator=(CloseStatus&&) noexcept = default;

    explicit CloseStatus(const Code& code);

    explicit CloseStatus(const std::uint16_t& value);

    [[nodiscard]] const std::uint16_t& value() const;
    std::uint16_t& value();

    [[nodiscard]] bool is_unknown() const;

    friend bool operator==(const CloseStatus& lv, const CloseStatus& rv);
    friend bool operator!=(const CloseStatus& lv, const CloseStatus& rv);

    virtual ~CloseStatus() = default;

protected:
    std::uint16_t value_ = 0xffff;
};

} // namespace rayalto::utils::network::websocket

#endif // RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_CLOSE_STATUS_H_
