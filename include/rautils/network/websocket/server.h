#ifndef RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_SERVER_H_
#define RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_SERVER_H_

#include <cstdint>
#include <functional>
#include <memory>

#include "rautils/network/general/cookie.h"
#include "rautils/network/general/header.h"
#include "rautils/network/general/url.h"
#include "rautils/network/websocket/close_status.h"
#include "rautils/network/websocket/message.h"

namespace rayalto::utils::network::websocket {

class Server {
public:
    // have to use ServerImpl, SessionWrapper in lws_callback,
    // so these cannot be private/protected
    class ServerImpl;
    class SessionWrapper;

    class Session;

    using ErrorCallback = std::function<void(Server&, const std::string&)>;
    using FilterNetworkCallback =
        std::function<void(Server&, const std::string&)>;
    using FilterProtocolCallback = std::function<void(Server&, Session&)>;
    using EstablishCallback = std::function<void(Server&, Session&)>;
    using ReceiveCallback =
        std::function<void(Server&, Session&, const Message&)>;
    using CloseCallback = std::function<void(
        Server&, Session&, const CloseStatus&, const std::string&)>;

    Server();
    Server(const Server&) = delete;
    Server(Server&&) noexcept;
    Server& operator=(const Server&) = delete;
    Server& operator=(Server&&) noexcept;

    virtual ~Server();

    // run server
    void start();
    void start(const general::Url& listen_on);
    void start(general::Url&& listen_on);

    // stop server
    void stop();
    void stop(const CloseStatus& close_status);
    void stop(const std::uint16_t& close_status);
    void stop(const std::string& message,
              const CloseStatus& close_status = CloseStatus {
                  CloseStatus::NORMAL});
    void stop(const std::string& message, const std::uint16_t& close_status);

    // if server was started
    [[nodiscard]] bool started() const;

    // listen_on, for example "ws://127.0.0.1:1080"
    const std::unique_ptr<std::string>& listen_on();
    void listen_on(const general::Url& listen_on);
    void listen_on(general::Url&& listen_on);
    void listen_on(std::nullptr_t);

    // websocket protocol name, sec-websocket-protocol
    const std::unique_ptr<std::string>& protocol();
    void protocol(const std::string& protocol);
    void protocol(std::string&& protocol);
    void protocol(std::nullptr_t);

    // server HTTP header
    const std::unique_ptr<general::Header>& header();
    void header(const general::Header& header);
    void header(general::Header&& header);
    void header(std::nullptr_t);

    // Cookie, equivalent to 'cookie' in http header
    const std::unique_ptr<general::Cookie>& cookie();
    void cookie(const general::Cookie& cookie);
    void cookie(general::Cookie&& cookie);
    void cookie(std::nullptr_t);

    /**
     * Called when the server failed. Params:
     *   - Server& server
     *   - const std::string& error_message
     */
    const std::unique_ptr<ErrorCallback>& on_error();
    void on_error(const ErrorCallback& callback);
    void on_error(ErrorCallback&& callback);
    void on_error(std::nullptr_t);

    /**
     * Called when a client connects to the server at network level,
     * you can decide whether to hang up or not, based on the client ip. Params:
     *   - Server& server
     *   - const std::string& client_ip
     */
    const std::unique_ptr<FilterNetworkCallback>& on_filter_network();
    void on_filter_network(const FilterNetworkCallback& callback);
    void on_filter_network(FilterNetworkCallback&& callback);
    void on_filter_network(std::nullptr_t);

    /**
     * Called when handshake has been received from client, but the response is
     * not sent yet, you can decide whether to disallow or not, base on the
     * header of client. Params:
     *   - Server& server
     *   - Session& client_session
     */
    const std::unique_ptr<FilterProtocolCallback>& on_filter_protocol();
    void on_filter_protocol(const FilterProtocolCallback& callback);
    void on_filter_protocol(FilterNetworkCallback&& callback);
    void on_filter_protocol(std::nullptr_t);

    /**
     * Called after the server completes the handshake with the incoming client.
     * Params:
     *   - Server& server
     *   - Session& client_session
     */
    const std::unique_ptr<EstablishCallback>& on_establish();
    void on_establish(const EstablishCallback& callback);
    void on_establish(EstablishCallback&& callback);
    void on_establish(std::nullptr_t);

    /**
     * Called when server received a message from a client. Params:
     *   - Server& server
     *   - Session& client_session
     *   - const Message& message_from_client
     */
    const std::unique_ptr<ReceiveCallback>& on_receive();
    void on_receive(const ReceiveCallback& callback);
    void on_receive(ReceiveCallback&& callback);
    void on_receive(std::nullptr_t);

    /**
     * Called when the session ends (could be initiated from client side or
     * server side). Params:
     *   - Server& server
     *   - Session& client_session
     *   - const CloseStatus& client_close_status
     *   - const std::string& client close_reason
     */
    const std::unique_ptr<CloseCallback>& on_close();
    void on_close(const CloseCallback& callback);
    void on_close(CloseCallback&& callback);
    void on_close(std::nullptr_t);

    // send message to all alive sessions
    void send(const Message& message);
    void send(Message&& message);

protected:
    std::unique_ptr<ServerImpl> impl_;
};

class Server::Session {
public:
    Session() = delete;
    Session(const Session&) = delete;
    Session(Session&&) noexcept = delete;
    Session& operator=(const Session&) = delete;
    Session& operator=(Session&&) noexcept = delete;

    explicit Session(SessionWrapper& wrapper);

    virtual ~Session() = default;

    // disconnect (server side)
    void disconnect();
    void disconnect(const CloseStatus& close_stataus);
    void disconnect(const std::uint16_t& close_status);
    void disconnect(const std::string& message,
                    const CloseStatus& close_status = CloseStatus {
                        CloseStatus::NORMAL});
    void disconnect(const std::string& message,
                    const std::uint16_t& close_status);

    // client ip
    const std::string& ip();

    // client header
    const std::unique_ptr<const general::Header>& client_header();

    // send to client
    Session& send(const Message& message);
    Session& send(Message& message);

protected:
    SessionWrapper& wrapper_;
};

} // namespace rayalto::utils::network::websocket

#endif // RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_SERVER_H_
