#ifndef RA_UTILS_WEBSOCKET_WS_CLIENT_H_
#define RA_UTILS_WEBSOCKET_WS_CLIENT_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_client.hpp"
#include "websocketpp/config/asio_no_tls_client.hpp"

#include "websocket/message_type.h"

namespace rayalto {
namespace utils {

class WsClient {
public:
    WsClient() = default;
    WsClient(const WsClient&) = delete;
    WsClient(WsClient&& client) noexcept;
    WsClient& operator=(const WsClient&) = delete;
    WsClient& operator=(WsClient&& client) noexcept;

    virtual ~WsClient();

    // receive callback
    void on_receive(const std::function<void(const websocket::MessageType&,
                                             const std::string&)>& callback);
    void on_receive(std::function<void(const websocket::MessageType&,
                                       const std::string&)>&& callback);

    // WebSocket server url, ws[s]://address[:port][/path]
    const std::string& url() const;
    void url(const std::string& url);
    void url(std::string&& url);

    // send message
    void send(const websocket::MessageType& type, const std::string& message);
    void send(const websocket::MessageType& type, std::string&& message);

    bool connected() const;

protected:
    bool with_ssl_ = true;
    std::shared_ptr<websocketpp::client<websocketpp::config::asio_tls_client>>
        client_ssl_ {nullptr};
    std::shared_ptr<websocketpp::client<websocketpp::config::asio_client>>
        client_no_ssl_ {nullptr};
    std::shared_ptr<std::thread> work_thread_;
    websocketpp::connection_hdl connection_handle_;
    std::function<void(const websocket::MessageType& message_type,
                       const std::string&)>
        receive_callback_;
    std::atomic<bool> connected_ {false};
    std::string url_;

    void release_();
    void connect_();
};

} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_WEBSOCKET_WS_CLIENT_H_
