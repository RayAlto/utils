#ifndef RA_UTILS_WEBSOCKET_WS_CLIENT_H_
#define RA_UTILS_WEBSOCKET_WS_CLIENT_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <system_error>
#include <thread>

#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_client.hpp"
#include "websocketpp/config/asio_no_tls_client.hpp"

#include "websocket/message_type.h"
#include "websocket/close_status.h"

namespace rayalto {
namespace utils {

class WsClient {
public:
    // default local close status
    static websocket::CloseStatus default_close_status;

public:
    WsClient() = default;
    WsClient(const WsClient&) = delete;
    WsClient(WsClient&& client) noexcept;
    WsClient& operator=(const WsClient&) = delete;
    WsClient& operator=(WsClient&& client) noexcept;

    virtual ~WsClient();

    // callback on connection established
    void on_establish(const std::function<void(WsClient&)>& callback);
    void on_establish(std::function<void(WsClient&)>&& callback);

    // callback on connection failed, 2nd param show local close status
    void on_fail(
        const std::function<void(WsClient&, const websocket::CloseStatus&)>&
            callback);
    void on_fail(std::function<void(WsClient&, const websocket::CloseStatus&)>&&
                     callback);

    // callback on message received
    void on_receive(const std::function<void(WsClient&,
                                             const websocket::MessageType&,
                                             const std::string&)>& callback);
    void on_receive(std::function<void(WsClient&,
                                       const websocket::MessageType&,
                                       const std::string&)>&& callback);

    // callback on connection closed, 2nd param show remote close status
    void on_close(
        const std::function<void(WsClient&, const websocket::CloseStatus&)>&
            callback);
    void on_close(
        std::function<void(WsClient&, const websocket::CloseStatus&)>&&
            callback);

    // connect
    void connect(std::error_code& error);

    // disconnect
    void disconnect();
    void disconnect(const websocket::CloseStatus& status);
    void disconnect(websocket::CloseStatus&& status);

    // WebSocket server url, ws[s]://address[:port][/path]
    const std::string& url() const;
    void url(const std::string& url);
    void url(std::string&& url);

    // send message
    void send(const websocket::MessageType& type,
              const std::string& message,
              std::error_code& error);
    void send(const websocket::MessageType& type,
              std::string&& message,
              std::error_code& error);

    bool connected() const;

protected:
    bool with_ssl_ = true;
    std::shared_ptr<websocketpp::client<websocketpp::config::asio_tls_client>>
        client_ssl_ {nullptr};
    std::shared_ptr<websocketpp::client<websocketpp::config::asio_client>>
        client_no_ssl_ {nullptr};
    std::shared_ptr<std::thread> work_thread_;
    websocketpp::connection_hdl connection_handle_;
    std::function<void(WsClient&)> establish_callback_;
    std::function<void(WsClient&, const websocket::CloseStatus&)>
        fail_callback_;
    std::function<void(WsClient&,
                       const websocket::MessageType& message_type,
                       const std::string&)>
        receive_callback_;
    std::function<void(WsClient&, const websocket::CloseStatus&)>
        close_callback_;
    std::atomic<bool> connected_ {false};
    std::string url_;

    void release_();
    void release_(const websocket::CloseStatus& status);
    void connect_(std::error_code& error);
};

} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_WEBSOCKET_WS_CLIENT_H_
