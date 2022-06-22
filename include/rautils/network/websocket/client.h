#ifndef RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_CLIENT_H_
#define RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_CLIENT_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "rautils/network/general/cookie.h"
#include "rautils/network/general/header.h"
#include "rautils/network/general/url.h"
#include "rautils/network/websocket/close_status.h"
#include "rautils/network/websocket/message.h"

namespace rayalto {
namespace utils {
namespace network {
namespace websocket {

class Client {
public:
    // have to use ClientImpl in lws_callback, so it cannot be private/protected
    class ClientImpl;

    using ErrorCallback = std::function<void(Client&, const std::string&)>;
    using EstablishCallback = std::function<void(Client&)>;
    using ReceiveCallback = std::function<void(Client&, const Message&)>;
    using CloseCallback =
        std::function<void(Client&, const CloseStatus&, const std::string&)>;

public:
    Client();
    Client(const Client&) = delete;
    Client(Client&&) noexcept;
    Client& operator=(const Client&) = delete;
    Client& operator=(Client&&) noexcept;

    virtual ~Client();

    Client& connect();
    Client& connect(const general::Url& url);
    Client& connect(general::Url&& url);

    Client& disconnect();
    Client& disconnect(const CloseStatus& close_status);
    Client& disconnect(const std::uint16_t& close_status);
    Client& disconnect(const std::string& message,
                    const CloseStatus& close_status = CloseStatus::NORMAL);
    Client& disconnect(const std::string& message,
                    const std::uint16_t& close_status);

    // if connection was established
    bool connected() const;

    // url
    const std::unique_ptr<general::Url>& url();
    Client& url(const general::Url& url);
    Client& url(general::Url&& url);
    Client& url(std::nullptr_t);

    // websocket protocol name, sec-websocket-protocol
    const std::unique_ptr<std::string>& protocol();
    Client& protocol(const std::string& protocol);
    Client& protocol(std::string&& protocol);
    Client& protocol(std::nullptr_t);

    // could be name, ip address
    const std::unique_ptr<std::string>& local_interface();
    Client& local_interface(const std::string& interface);
    Client& local_interface(std::string&& interface);
    Client& local_interface(std::nullptr_t);

    // HTTP header
    const std::unique_ptr<general::Header>& header();
    Client& header(const general::Header& header);
    Client& header(general::Header&& header);
    Client& header(std::nullptr_t);

    // Cookie, equivalent to 'cookie' in http header
    const std::unique_ptr<general::Cookie>& cookie();
    Client& cookie(const general::Cookie& cookie);
    Client& cookie(general::Cookie&& cookie);
    Client& cookie(std::nullptr_t);

    // callback on connection error
    const std::unique_ptr<ErrorCallback>& on_error();
    Client& on_error(const ErrorCallback& callback);
    Client& on_error(ErrorCallback&& callback);
    Client& on_error(std::nullptr_t);

    // callback on connection establishment
    const std::unique_ptr<EstablishCallback>& on_establish();
    Client& on_establish(const EstablishCallback& callback);
    Client& on_establish(EstablishCallback&& callback);
    Client& on_establish(std::nullptr_t);

    // callback on receiving message
    const std::unique_ptr<ReceiveCallback>& on_receive();
    Client& on_receive(const ReceiveCallback& callback);
    Client& on_receive(ReceiveCallback&& callback);
    Client& on_receive(std::nullptr_t);

    // callback on connection closure
    const std::unique_ptr<CloseCallback>& on_close();
    Client& on_close(const CloseCallback& callback);
    Client& on_close(CloseCallback&& callback);
    Client& on_close(std::nullptr_t);

    // send message to server
    Client& send(const Message& message);
    Client& send(Message&& message);

protected:
    std::unique_ptr<ClientImpl> impl_;
};

} // namespace websocket
} // namespace network
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_H_
