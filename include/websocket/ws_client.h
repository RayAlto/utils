#ifndef RA_UTILS_WEBSOCKET_WS_CLIENT_H_
#define RA_UTILS_WEBSOCKET_WS_CLIENT_H_

#include <functional>
#include <memory>
#include <system_error>

#include "websocket/close_status.h"
#include "websocket/message_type.h"

namespace rayalto {
namespace utils {

class WsClient {
public:
    WsClient();
    WsClient(const WsClient&) = delete;
    WsClient(WsClient&&) noexcept = default;
    WsClient& operator=(const WsClient&) = delete;
    WsClient& operator=(WsClient&&) noexcept = default;

    virtual ~WsClient();

    // default local close status
    static websocket::CloseStatus& default_close_status();

    // callback on connection established
    WsClient& on_establish(const std::function<void(WsClient&)>& callback);
    WsClient& on_establish(std::function<void(WsClient&)>&& callback);

    // callback on connection failed, 2nd param show local close status
    WsClient& on_fail(
        const std::function<void(WsClient&, const websocket::CloseStatus&)>&
            callback);
    WsClient& on_fail(std::function<void(WsClient&, const websocket::CloseStatus&)>&&
                     callback);

    // callback on message received
    WsClient& on_receive(const std::function<void(WsClient&,
                                             const websocket::MessageType&,
                                             const std::string&)>& callback);
    WsClient& on_receive(std::function<void(WsClient&,
                                       const websocket::MessageType&,
                                       const std::string&)>&& callback);

    // callback on connection closed, 2nd param show remote close status
    WsClient& on_close(
        const std::function<void(WsClient&, const websocket::CloseStatus&)>&
            callback);
    WsClient& on_close(
        std::function<void(WsClient&, const websocket::CloseStatus&)>&&
            callback);

    // connect
    WsClient& connect(std::error_code& error);

    // disconnect
    WsClient& disconnect();
    WsClient& disconnect(const websocket::CloseStatus& status);
    WsClient& disconnect(websocket::CloseStatus&& status);

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
    WsClient& send(const websocket::MessageType& type,
                   const std::string& message,
                   std::error_code& error);
    WsClient& send(const websocket::MessageType& type,
                   std::string&& message,
                   std::error_code& error);

    bool connected() const;

protected:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_WEBSOCKET_WS_CLIENT_H_
