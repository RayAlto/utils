#define ASIO_STANDALONE

#include "rautils/network/ws_client.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <system_error>
#include <thread>
#include <utility>

#include "asio/ssl/context.hpp"
#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_client.hpp"
#include "websocketpp/config/asio_no_tls_client.hpp"
#include "websocketpp/close.hpp"

namespace rayalto {
namespace utils {
namespace network {

class WsClient::Impl {
public:
    static CloseStatus default_close_status;

public:
    Impl(WsClient& client);
    Impl() = delete;
    Impl(const Impl&) = delete;
    Impl(Impl&& client) noexcept;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&& client) noexcept;

    virtual ~Impl();

    void on_establish(const std::function<void(WsClient&)>& callback);
    void on_establish(std::function<void(WsClient&)>&& callback);

    void on_fail(
        const std::function<void(WsClient&, const CloseStatus&)>& callback);
    void on_fail(std::function<void(WsClient&, const CloseStatus&)>&& callback);

    void on_receive(
        const std::function<
            void(WsClient&, const MessageType&, const std::string&)>& callback);
    void on_receive(
        std::function<void(WsClient&, const MessageType&, const std::string&)>&&
            callback);

    void on_close(
        const std::function<void(WsClient&, const CloseStatus&)>& callback);
    void on_close(
        std::function<void(WsClient&, const CloseStatus&)>&& callback);

    void connect(std::error_code& error);

    void disconnect();
    void disconnect(const CloseStatus& status);
    void disconnect(CloseStatus&& status);

    void url(const std::string& url);
    void url(std::string&& url);
    const std::string& url() const;

    void useragent(const std::string& useragent);
    void useragent(std::string&& useragent);
    std::string& useragent();
    const std::string& useragent() const;

    void send(const MessageType& type,
              const std::string& message,
              std::error_code& error);
    void send(const MessageType& type,
              std::string&& message,
              std::error_code& error);

    bool connected() const;

protected:
    std::reference_wrapper<WsClient> client_;

    bool with_ssl_ = true;

    std::unique_ptr<websocketpp::client<websocketpp::config::asio_tls_client>>
        client_ssl_ {nullptr};
    std::unique_ptr<websocketpp::client<websocketpp::config::asio_client>>
        client_no_ssl_ {nullptr};

    std::unique_ptr<std::thread> work_thread_;

    websocketpp::connection_hdl connection_handle_;

    std::function<void(WsClient&)> establish_callback_;
    std::function<void(WsClient&, const CloseStatus&)> fail_callback_;
    std::function<void(
        WsClient&, const MessageType& message_type, const std ::string&)>
        receive_callback_;
    std::function<void(WsClient&, const CloseStatus&)> close_callback_;

    std::atomic<bool> connected_ {false};

    std::string url_;
    std::string useragent_;

    void release_();
    void release_(const CloseStatus& status);
    void release_work_thread_();
    void connect_(std::error_code& error);
    void connect_no_ssl_(std::error_code& error);
    void connect_ssl_(std::error_code& error);
};

WsClient::CloseStatus WsClient::Impl::default_close_status(
    CloseStatus::Code::NORMAL);

WsClient::Impl::Impl(WsClient& client) : client_(client) {}

WsClient::Impl::Impl(WsClient::Impl&& client) noexcept :
    client_(client.client_),
    with_ssl_(std::move(client.with_ssl_)),
    client_ssl_(std::move(client.client_ssl_)),
    client_no_ssl_(std::move(client.client_no_ssl_)),
    work_thread_(std::move(client.work_thread_)),
    connection_handle_(std::move(client.connection_handle_)),
    receive_callback_(std::move(client.receive_callback_)),
    connected_(client.connected_.load()),
    url_(std::move(client.url_)) {}

WsClient::Impl& WsClient::Impl::operator=(WsClient::Impl&& client) noexcept {
    client_ = client.client_;
    with_ssl_ = std::move(client.with_ssl_);
    client_ssl_ = std::move(client.client_ssl_);
    client_no_ssl_ = std::move(client.client_no_ssl_);
    work_thread_ = std::move(client.work_thread_);
    connection_handle_ = std::move(client.connection_handle_);
    receive_callback_ = std::move(client.receive_callback_);
    connected_ = client.connected_.load();
    url_ = std::move(client.url_);
    return *this;
}

WsClient::Impl::~Impl() {
    release_();
}

void WsClient::Impl::on_establish(
    const std::function<void(WsClient&)>& callback) {
    establish_callback_ = callback;
}

void WsClient::Impl::on_establish(std::function<void(WsClient&)>&& callback) {
    establish_callback_ = std::move(callback);
}

void WsClient::Impl::on_fail(
    const std::function<void(WsClient&, const CloseStatus&)>& callback) {
    fail_callback_ = callback;
}

void WsClient::Impl::on_fail(
    std::function<void(WsClient&, const CloseStatus&)>&& callback) {
    fail_callback_ = std::move(callback);
}

void WsClient::Impl::on_receive(
    const std::function<
        void(WsClient&, const MessageType&, const std::string&)>& callback) {
    receive_callback_ = callback;
}

void WsClient::Impl::on_receive(
    std::function<void(WsClient&, const MessageType&, const std::string&)>&&
        callback) {
    receive_callback_ = std::move(callback);
}

void WsClient::Impl::on_close(
    const std::function<void(WsClient&, const CloseStatus&)>& callback) {
    close_callback_ = callback;
}

void WsClient::Impl::on_close(
    std::function<void(WsClient&, const CloseStatus&)>&& callback) {
    close_callback_ = std::move(callback);
}

void WsClient::Impl::connect(std::error_code& error) {
    release_();
    connect_(error);
}

void WsClient::Impl::disconnect() {
    release_();
}

void WsClient::Impl::disconnect(const CloseStatus& status) {
    release_(status);
}

void WsClient::Impl::disconnect(CloseStatus&& status) {
    release_(std::move(status));
}

void WsClient::Impl::url(std::string&& url) {
    url_ = std::move(url);
}

const std::string& WsClient::Impl::url() const {
    return url_;
}

void WsClient::Impl::url(const std::string& url) {
    url_ = url;
}

void WsClient::Impl::useragent(const std::string& useragent) {
    useragent_ = useragent;
}

void WsClient::Impl::useragent(std::string&& useragent) {
    useragent_ = std::move(useragent);
}

std::string& WsClient::Impl::useragent() {
    return useragent_;
}

const std::string& WsClient::Impl::useragent() const {
    return useragent_;
}

void WsClient::Impl::send(const MessageType& type,
                          const std::string& message,
                          std::error_code& error) {
    if (!connected_) {
        return;
    }
    if (with_ssl_) {
        switch (type) {
        case MessageType::TEXT:
            client_ssl_->send(connection_handle_,
                              message,
                              websocketpp::frame::opcode::text,
                              error);
            break;
        case MessageType::BINARY:
            client_ssl_->send(connection_handle_,
                              message,
                              websocketpp::frame::opcode::binary,
                              error);
            break;
        default: break;
        }
    }
    else if (!with_ssl_) {
        switch (type) {
        case MessageType::TEXT:
            client_no_ssl_->send(connection_handle_,
                                 message,
                                 websocketpp::frame::opcode::text,
                                 error);
            break;
        case MessageType::BINARY:
            client_no_ssl_->send(connection_handle_,
                                 message,
                                 websocketpp::frame::opcode::binary,
                                 error);
            break;
        default: break;
        }
    }
}

void WsClient::Impl::send(const MessageType& type,
                          std::string&& message,
                          std::error_code& error) {
    if (!connected_) {
        return;
    }
    if (with_ssl_) {
        switch (type) {
        case MessageType::TEXT:
            client_ssl_->send(connection_handle_,
                              std::move(message),
                              websocketpp::frame::opcode::text,
                              error);
            break;
        case MessageType::BINARY:
            client_ssl_->send(connection_handle_,
                              std::move(message),
                              websocketpp::frame::opcode::binary,
                              error);
            break;
        default: break;
        }
    }
    else if (!with_ssl_) {
        switch (type) {
        case MessageType::TEXT:
            client_no_ssl_->send(connection_handle_,
                                 std::move(message),
                                 websocketpp::frame::opcode::text,
                                 error);
            break;
        case MessageType::BINARY:
            client_no_ssl_->send(connection_handle_,
                                 std::move(message),
                                 websocketpp::frame::opcode::binary,
                                 error);
            break;
        default: break;
        }
    }
}

bool WsClient::Impl::connected() const {
    return connected_;
}

void WsClient::Impl::release_() {
    release_(default_close_status);
}

void WsClient::Impl::release_(const CloseStatus& status) {
    // release websocket client
    if (with_ssl_ && client_ssl_ != nullptr) {
        if (connected_) {
            client_ssl_->close(connection_handle_, status.code, status.reason);
        }
        client_ssl_->stop_perpetual();
        release_work_thread_();
        client_ssl_.reset();
    }
    else if (!with_ssl_ && client_no_ssl_ != nullptr) {
        if (connected_) {
            client_no_ssl_->close(
                connection_handle_, status.code, status.reason);
        }
        client_no_ssl_->stop_perpetual();
        release_work_thread_();
        client_no_ssl_.reset();
    }
    // release work thread
    connected_ = false;
}

void WsClient::Impl::release_work_thread_() {
    if (work_thread_ != nullptr) {
        if (work_thread_->joinable()) {
            work_thread_->join();
        }
        work_thread_.reset();
    }
}

void WsClient::Impl::connect_(std::error_code& error) {
    if (url_[2] == 's') {
        // wss
        connect_ssl_(error);
    }
    else {
        // ws
        connect_no_ssl_(error);
    }
}

void WsClient::Impl::connect_no_ssl_(std::error_code& error) {
    with_ssl_ = false;
    client_no_ssl_ = std::make_unique<
        websocketpp::client<websocketpp::config::asio_client>>();
    client_no_ssl_->clear_access_channels(websocketpp::log::alevel::all);
    client_no_ssl_->clear_error_channels(websocketpp::log::elevel::all);
    client_no_ssl_->set_user_agent(useragent_.empty() ? "RaWebsocket/114514"
                                                      : useragent_);
    client_no_ssl_->init_asio();

    client_no_ssl_->set_open_handler(
        [&](websocketpp::connection_hdl handle) -> void {
            connected_ = true;
            if (establish_callback_) {
                establish_callback_(client_);
            }
        });

    client_no_ssl_->set_fail_handler([&](websocketpp::connection_hdl handle)
                                         -> void {
        connected_ = false;
        websocketpp::client<websocketpp::config::asio_client>::connection_ptr
            connection = client_no_ssl_->get_con_from_hdl(handle);
        if (fail_callback_) {
            std::uint16_t local_close_code = connection->get_local_close_code();
            fail_callback_(client_,
                           CloseStatus(local_close_code,
                                       websocketpp::close::status::get_string(
                                           local_close_code),
                                       connection->get_local_close_reason()));
        }
    });

    client_no_ssl_->set_close_handler([&](websocketpp::connection_hdl handle)
                                          -> void {
        connected_ = false;
        websocketpp::client<websocketpp::config::asio_client>::connection_ptr
            connection = client_no_ssl_->get_con_from_hdl(handle);
        if (close_callback_) {
            std::uint16_t remote_close_code =
                connection->get_remote_close_code();
            close_callback_(client_,
                            CloseStatus(remote_close_code,
                                        websocketpp::close::status::get_string(
                                            remote_close_code),
                                        connection->get_remote_close_reason()));
        }
    });

    client_no_ssl_->set_message_handler(
        [&](websocketpp::connection_hdl handle,
            websocketpp::client<websocketpp::config::asio_client>::message_ptr
                message) -> void {
            if (!receive_callback_) {
                return;
            }
            switch (message->get_opcode()) {
            case websocketpp::frame::opcode::text:
                receive_callback_(
                    client_, MessageType::TEXT, message->get_payload());
                break;
            case websocketpp::frame::opcode::binary:
                receive_callback_(
                    client_, MessageType::BINARY, message->get_payload());
                break;
            default: break;
            }
        });

    client_no_ssl_->start_perpetual();
    work_thread_ = std::make_unique<std::thread>(
        &websocketpp::client<websocketpp::config::asio_client>::run,
        client_no_ssl_.get());

    // get connection
    websocketpp::client<websocketpp::config::asio_client>::connection_ptr
        connection = client_no_ssl_->get_connection(url_, error);

    connection_handle_ = connection->get_handle();

    client_no_ssl_->connect(connection);
}

void WsClient::Impl::connect_ssl_(std::error_code& error) {
    with_ssl_ = true;
    client_ssl_ = std::make_unique<
        websocketpp::client<websocketpp::config::asio_tls_client>>();
    client_ssl_->clear_access_channels(websocketpp::log::alevel::all);
    client_ssl_->clear_error_channels(websocketpp::log::elevel::all);
    client_ssl_->set_user_agent(useragent_.empty() ? "RaWebsocket/114514"
                                                   : useragent_);
    client_ssl_->init_asio();

    client_ssl_->set_tls_init_handler(
        [&](websocketpp::connection_hdl handle)
            -> std::shared_ptr<asio::ssl::context> {
            std::shared_ptr<asio::ssl::context> ssl_context =
                std::make_shared<asio::ssl::context>(
                    asio::ssl::context::sslv23);
            ssl_context->set_options(asio::ssl::context::default_workarounds
                                     | asio::ssl::context::no_sslv2
                                     | asio::ssl::context::no_sslv3
                                     | asio::ssl::context::single_dh_use);
            return ssl_context;
        });

    client_ssl_->set_open_handler(
        [&](websocketpp::connection_hdl handle) -> void {
            connected_ = true;
            if (establish_callback_) {
                establish_callback_(client_);
            }
        });

    client_ssl_->set_fail_handler([&](websocketpp::connection_hdl handle)
                                      -> void {
        connected_ = false;
        websocketpp::client<
            websocketpp::config::asio_tls_client>::connection_ptr connection =
            client_ssl_->get_con_from_hdl(handle);
        if (fail_callback_) {
            std::uint16_t local_close_code = connection->get_local_close_code();
            fail_callback_(client_,
                           CloseStatus(local_close_code,
                                       websocketpp::close::status::get_string(
                                           local_close_code),
                                       connection->get_local_close_reason()));
        }
    });

    client_ssl_->set_close_handler([&](websocketpp::connection_hdl handle)
                                       -> void {
        connected_ = false;
        websocketpp::client<
            websocketpp::config::asio_tls_client>::connection_ptr connection =
            client_ssl_->get_con_from_hdl(handle);
        if (close_callback_) {
            std::uint16_t remote_close_code =
                connection->get_remote_close_code();
            close_callback_(client_,
                            CloseStatus(remote_close_code,
                                        websocketpp::close::status::get_string(
                                            remote_close_code),
                                        connection->get_remote_close_reason()));
        }
    });

    client_ssl_->set_message_handler(
        [&](websocketpp::connection_hdl handle,
            websocketpp::client<websocketpp::config::asio_tls_client>::
                message_ptr message) -> void {
            if (!receive_callback_) {
                return;
            }
            switch (message->get_opcode()) {
            case websocketpp::frame::opcode::text:
                receive_callback_(
                    client_, MessageType::TEXT, message->get_payload());
                break;
            case websocketpp::frame::opcode::binary:
                receive_callback_(
                    client_, MessageType::BINARY, message->get_payload());
                break;
            default: break;
            }
        });

    client_ssl_->start_perpetual();
    work_thread_ = std::make_unique<std::thread>(
        &websocketpp::client<websocketpp::config::asio_tls_client>::run,
        client_ssl_.get());

    websocketpp::client<websocketpp::config::asio_tls_client>::connection_ptr
        connection = client_ssl_->get_connection(url_, error);

    connection_handle_ = connection->get_handle();

    client_ssl_->connect(connection);
}

WsClient::WsClient() : impl_(std::make_unique<Impl>(*this)) {}

WsClient::~WsClient() = default;

WsClient::CloseStatus& WsClient::default_close_status() {
    return Impl::default_close_status;
}

WsClient& WsClient::on_establish(
    const std::function<void(WsClient&)>& callback) {
    impl_->on_establish(callback);
    return *this;
}

WsClient& WsClient::on_establish(std::function<void(WsClient&)>&& callback) {
    impl_->on_establish(std::move(callback));
    return *this;
}

WsClient& WsClient::on_fail(
    const std::function<void(WsClient&, const CloseStatus&)>& callback) {
    impl_->on_fail(callback);
    return *this;
}

WsClient& WsClient::on_fail(
    std::function<void(WsClient&, const CloseStatus&)>&& callback) {
    impl_->on_fail(std::move(callback));
    return *this;
}

WsClient& WsClient::on_receive(
    const std::function<
        void(WsClient&, const MessageType&, const std::string&)>& callback) {
    impl_->on_receive(callback);
    return *this;
}

WsClient& WsClient::on_receive(
    std::function<void(WsClient&, const MessageType&, const std::string&)>&&
        callback) {
    impl_->on_receive(std::move(callback));
    return *this;
}

WsClient& WsClient::on_close(
    const std::function<void(WsClient&, const CloseStatus&)>& callback) {
    impl_->on_close(callback);
    return *this;
}

WsClient& WsClient::on_close(
    std::function<void(WsClient&, const CloseStatus&)>&& callback) {
    impl_->on_close(std::move(callback));
    return *this;
}

WsClient& WsClient::connect(std::error_code& error) {
    impl_->connect(error);
    return *this;
}

WsClient& WsClient::disconnect() {
    impl_->disconnect();
    return *this;
}

WsClient& WsClient::disconnect(const CloseStatus& status) {
    impl_->disconnect(status);
    return *this;
}

WsClient& WsClient::disconnect(CloseStatus&& status) {
    impl_->disconnect(std::move(status));
    return *this;
}

WsClient& WsClient::url(const std::string& url) {
    impl_->url(url);
    return *this;
}

WsClient& WsClient::url(std::string&& url) {
    impl_->url(std::move(url));
    return *this;
}

const std::string& WsClient::url() const {
    return impl_->url();
}

WsClient& WsClient::useragent(const std::string& useragent) {
    impl_->useragent(useragent);
    return *this;
}

WsClient& WsClient::useragent(std::string&& useragent) {
    impl_->useragent(std::move(useragent));
    return *this;
}

std::string& WsClient::useragent() {
    return impl_->useragent();
}

const std::string& WsClient::useragent() const {
    return impl_->useragent();
}

WsClient& WsClient::send(const MessageType& type,
                         const std::string& message,
                         std::error_code& error) {
    impl_->send(type, message, error);
    return *this;
}

WsClient& WsClient::send(const MessageType& type,
                         std::string&& message,
                         std::error_code& error) {
    impl_->send(type, std::move(message), error);
    return *this;
}

bool WsClient::connected() const {
    return impl_->connected();
}

} // namespace network
} // namespace utils
} // namespace rayalto
