#ifndef RA_UTILS_WEBSOCKET_WS_CLIENT_H_
#define RA_UTILS_WEBSOCKET_WS_CLIENT_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <system_error>

namespace rayalto {
namespace utils {
namespace network {

class WsClient {
public:
    enum class MessageType : std::uint8_t { TEXT, BINARY };
    struct CloseStatus;

public:
    WsClient();
    WsClient(const WsClient&) = delete;
    WsClient(WsClient&&) noexcept = default;
    WsClient& operator=(const WsClient&) = delete;
    WsClient& operator=(WsClient&&) noexcept = default;

    virtual ~WsClient();

    // default local close status
    static CloseStatus& default_close_status();

    // callback on connection established
    WsClient& on_establish(const std::function<void(WsClient&)>& callback);
    WsClient& on_establish(std::function<void(WsClient&)>&& callback);

    // callback on connection failed, 2nd param show local close status
    WsClient& on_fail(
        const std::function<void(WsClient&, const CloseStatus&)>& callback);
    WsClient& on_fail(
        std::function<void(WsClient&, const CloseStatus&)>&& callback);

    // callback on message received
    WsClient& on_receive(
        const std::function<
            void(WsClient&, const MessageType&, const std::string&)>& callback);
    WsClient& on_receive(
        std::function<void(WsClient&, const MessageType&, const std::string&)>&&
            callback);

    // callback on connection closed, 2nd param show remote close status
    WsClient& on_close(
        const std::function<void(WsClient&, const CloseStatus&)>& callback);
    WsClient& on_close(
        std::function<void(WsClient&, const CloseStatus&)>&& callback);

    // connect
    WsClient& connect(std::error_code& error);

    // disconnect
    WsClient& disconnect();
    WsClient& disconnect(const CloseStatus& status);
    WsClient& disconnect(CloseStatus&& status);

    // WebSocket server url, ws[s]://address[:port][/path]
    WsClient& url(const std::string& url);
    WsClient& url(std::string&& url);
    const std::string& url() const;

    // User-Agent
    WsClient& useragent(const std::string& useragent);
    WsClient& useragent(std::string&& useragent);
    std::string& useragent();
    const std::string& useragent() const;

    // send message
    WsClient& send(const MessageType& type,
                   const std::string& message,
                   std::error_code& error);
    WsClient& send(const MessageType& type,
                   std::string&& message,
                   std::error_code& error);

    bool connected() const;

protected:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

struct WsClient::CloseStatus {
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

} // namespace network
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_WEBSOCKET_WS_CLIENT_H_
