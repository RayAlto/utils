#define ASIO_STANDALONE

#include "websocket/ws_client.h"

#include <cstdint>
#include <memory>
#include <system_error>
#include <thread>
#include <utility>
#include <websocketpp/close.hpp>

#include "asio/ssl/context.hpp"
#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_client.hpp"
#include "websocketpp/config/asio_no_tls_client.hpp"

#include "websocket/message_type.h"
#include "websocket/close_status.h"

namespace rayalto {
namespace utils {

websocket::CloseStatus WsClient::default_close_status(
    websocket::close_status::Code::NORMAL);

WsClient::WsClient(WsClient&& client) noexcept :
    with_ssl_(std::move(client.with_ssl_)),
    client_ssl_(std::move(client.client_ssl_)),
    client_no_ssl_(std::move(client.client_no_ssl_)),
    work_thread_(std::move(client.work_thread_)),
    connection_handle_(std::move(client.connection_handle_)),
    receive_callback_(std::move(client.receive_callback_)),
    connected_(client.connected_.load()),
    url_(std::move(client.url_)) {}

WsClient& WsClient::operator=(WsClient&& client) noexcept {
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

WsClient::~WsClient() {
    release_();
}

void WsClient::on_establish(const std::function<void(WsClient&)>& callback) {
    establish_callback_ = callback;
}

void WsClient::on_establish(std::function<void(WsClient&)>&& callback) {
    establish_callback_ = std::move(callback);
}

void WsClient::on_fail(
    const std::function<void(WsClient&, const websocket::CloseStatus&)>&
        callback) {
    fail_callback_ = callback;
}

void WsClient::on_fail(
    std::function<void(WsClient&, const websocket::CloseStatus&)>&& callback) {
    fail_callback_ = std::move(callback);
}

void WsClient::on_receive(
    const std::function<void(WsClient&,
                             const websocket::MessageType&,
                             const std::string&)>& callback) {
    receive_callback_ = callback;
}

void WsClient::on_receive(std::function<void(WsClient&,
                                             const websocket::MessageType&,
                                             const std::string&)>&& callback) {
    receive_callback_ = std::move(callback);
}

void WsClient::on_close(
    const std::function<void(WsClient&, const websocket::CloseStatus&)>&
        callback) {
    close_callback_ = callback;
}

void WsClient::on_close(
    std::function<void(WsClient&, const websocket::CloseStatus&)>&& callback) {
    close_callback_ = std::move(callback);
}

void WsClient::connect(std::error_code& error) {
    release_();
    connect_(error);
}

void WsClient::disconnect() {
    release_();
}

void WsClient::disconnect(const websocket::CloseStatus& status) {
    release_(status);
}

void WsClient::disconnect(websocket::CloseStatus&& status) {
    release_(std::move(status));
}

const std::string& WsClient::url() const {
    return url_;
}

void WsClient::url(const std::string& url) {
    url_ = url;
}

void WsClient::url(std::string&& url) {
    url_ = std::move(url);
}

void WsClient::send(const websocket::MessageType& type,
                    const std::string& message,
                    std::error_code& error) {
    if (!connected_) {
        return;
    }
    if (with_ssl_) {
        switch (type) {
        case websocket::MessageType::TEXT:
            client_ssl_->send(connection_handle_,
                              std::move(message),
                              websocketpp::frame::opcode::text,
                              error);
            break;
        case websocket::MessageType::BINARY:
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
        case websocket::MessageType::TEXT:
            client_no_ssl_->send(connection_handle_,
                                 std::move(message),
                                 websocketpp::frame::opcode::text,
                                 error);
            break;
        case websocket::MessageType::BINARY:
            client_no_ssl_->send(connection_handle_,
                                 std::move(message),
                                 websocketpp::frame::opcode::binary,
                                 error);
            break;
        default: break;
        }
    }
}

void WsClient::send(const websocket::MessageType& type,
                    std::string&& message,
                    std::error_code& error) {
    if (!connected_) {
        return;
    }
    if (with_ssl_) {
        switch (type) {
        case websocket::MessageType::TEXT:
            client_ssl_->send(connection_handle_,
                              message,
                              websocketpp::frame::opcode::text,
                              error);
            break;
        case websocket::MessageType::BINARY:
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
        case websocket::MessageType::TEXT:
            client_no_ssl_->send(connection_handle_,
                                 message,
                                 websocketpp::frame::opcode::text,
                                 error);
            break;
        case websocket::MessageType::BINARY:
            client_no_ssl_->send(connection_handle_,
                                 message,
                                 websocketpp::frame::opcode::binary,
                                 error);
            break;
        default: break;
        }
    }
}

bool WsClient::connected() const {
    return connected_;
}

void WsClient::release_() {
    release_(default_close_status);
}

void WsClient::release_(const websocket::CloseStatus& status) {
    // release websocket client
    if (with_ssl_ && client_ssl_) {
        if (connected_) {
            client_ssl_->close(connection_handle_, status.code, status.reason);
        }
        client_ssl_->stop_perpetual();
        client_ssl_.reset();
    }
    else if (!with_ssl_ && client_no_ssl_) {
        if (connected_) {
            client_no_ssl_->close(
                connection_handle_, status.code, status.reason);
        }
        client_no_ssl_->stop_perpetual();
        client_no_ssl_.reset();
    }
    // release work thread
    if (work_thread_) {
        if (work_thread_->joinable()) {
            work_thread_->join();
        }
        work_thread_.reset();
    }
    connected_ = false;
}

void WsClient::connect_(std::error_code& error) {
    if (url_[2] == 's') {
        // wss

        with_ssl_ = true;
        client_ssl_ = std::make_shared<
            websocketpp::client<websocketpp::config::asio_tls_client>>();
        client_ssl_->clear_access_channels(websocketpp::log::alevel::all);
        client_ssl_->clear_error_channels(websocketpp::log::elevel::all);
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
                    establish_callback_(*this);
                }
            });

        client_ssl_->set_fail_handler(
            [&](websocketpp::connection_hdl handle) -> void {
                connected_ = false;
                websocketpp::client<websocketpp::config::asio_tls_client>::
                    connection_ptr connection =
                        client_ssl_->get_con_from_hdl(handle);
                if (fail_callback_) {
                    std::uint16_t local_close_code =
                        connection->get_local_close_code();
                    fail_callback_(*this,
                                   websocket::CloseStatus(
                                       local_close_code,
                                       websocketpp::close::status::get_string(
                                           local_close_code),
                                       connection->get_local_close_reason()));
                }
            });

        client_ssl_->set_close_handler(
            [&](websocketpp::connection_hdl handle) -> void {
                connected_ = false;
                websocketpp::client<websocketpp::config::asio_tls_client>::
                    connection_ptr connection =
                        client_ssl_->get_con_from_hdl(handle);
                if (close_callback_) {
                    std::uint16_t remote_close_code =
                        connection->get_remote_close_code();
                    close_callback_(*this,
                                    websocket::CloseStatus(
                                        remote_close_code,
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
                    receive_callback_(*this,
                                      websocket::MessageType::TEXT,
                                      message->get_payload());
                    break;
                case websocketpp::frame::opcode::binary:
                    receive_callback_(*this,
                                      websocket::MessageType::BINARY,
                                      message->get_payload());
                    break;
                default: break;
                }
            });

        client_ssl_->start_perpetual();
        work_thread_ = std::make_shared<std::thread>(
            &websocketpp::client<websocketpp::config::asio_tls_client>::run,
            client_ssl_);

        websocketpp::client<
            websocketpp::config::asio_tls_client>::connection_ptr connection =
            client_ssl_->get_connection(url_, error);

        connection_handle_ = connection->get_handle();

        client_ssl_->connect(connection);
    }
    else {
        // ws

        with_ssl_ = false;
        client_no_ssl_ = std::make_shared<
            websocketpp::client<websocketpp::config::asio_client>>();
        client_no_ssl_->clear_access_channels(websocketpp::log::alevel::all);
        client_no_ssl_->clear_error_channels(websocketpp::log::elevel::all);
        client_no_ssl_->init_asio();

        client_no_ssl_->set_open_handler(
            [&](websocketpp::connection_hdl handle) -> void {
                connected_ = true;
                if (establish_callback_) {
                    establish_callback_(*this);
                }
            });

        client_no_ssl_->set_fail_handler([&](websocketpp::connection_hdl handle)
                                             -> void {
            connected_ = false;
            websocketpp::client<
                websocketpp::config::asio_client>::connection_ptr connection =
                client_no_ssl_->get_con_from_hdl(handle);
            if (fail_callback_) {
                std::uint16_t local_close_code =
                    connection->get_local_close_code();
                fail_callback_(*this,
                               websocket::CloseStatus(
                                   local_close_code,
                                   websocketpp::close::status::get_string(
                                       local_close_code),
                                   connection->get_local_close_reason()));
            }
        });

        client_no_ssl_->set_close_handler(
            [&](websocketpp::connection_hdl handle) -> void {
                connected_ = false;
                websocketpp::client<websocketpp::config::asio_client>::
                    connection_ptr connection =
                        client_no_ssl_->get_con_from_hdl(handle);
                if (close_callback_) {
                    std::uint16_t remote_close_code =
                        connection->get_remote_close_code();
                    close_callback_(*this,
                                    websocket::CloseStatus(
                                        remote_close_code,
                                        websocketpp::close::status::get_string(
                                            remote_close_code),
                                        connection->get_remote_close_reason()));
                }
            });

        client_no_ssl_->set_message_handler(
            [&](websocketpp::connection_hdl handle,
                websocketpp::client<websocketpp::config::asio_client>::
                    message_ptr message) -> void {
                if (!receive_callback_) {
                    return;
                }
                switch (message->get_opcode()) {
                case websocketpp::frame::opcode::text:
                    receive_callback_(*this,
                                      websocket::MessageType::TEXT,
                                      message->get_payload());
                    break;
                case websocketpp::frame::opcode::binary:
                    receive_callback_(*this,
                                      websocket::MessageType::BINARY,
                                      message->get_payload());
                    break;
                default: break;
                }
            });

        client_no_ssl_->start_perpetual();
        work_thread_ = std::make_shared<std::thread>(
            &websocketpp::client<websocketpp::config::asio_client>::run,
            client_no_ssl_);

        // get connection
        websocketpp::client<websocketpp::config::asio_client>::connection_ptr
            connection = client_no_ssl_->get_connection(url_, error);

        connection_handle_ = connection->get_handle();

        client_no_ssl_->connect(connection);
    }
}

} // namespace utils
} // namespace rayalto
