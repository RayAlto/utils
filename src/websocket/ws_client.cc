#define ASIO_STANDALONE

#include <iostream>

#include "websocket/ws_client.h"

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

namespace rayalto {
namespace utils {

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

void WsClient::on_receive(
    const std::function<void(const websocket::MessageType&,
                             const std::string&)>& callback) {
    receive_callback_ = callback;
}

void WsClient::on_receive(std::function<void(const websocket::MessageType&,
                                             const std::string&)>&& callback) {
    receive_callback_ = std::move(callback);
}

const std::string& WsClient::url() const {
    return url_;
}

void WsClient::url(const std::string& url) {
    release_();
    url_ = url;
    connect_();
}

void WsClient::url(std::string&& url) {
    release_();
    url_ = std::move(url);
    connect_();
}

void WsClient::send(const websocket::MessageType& type,
                    const std::string& message) {
    if (!connected_) {
        return;
    }
    std::error_code ws_error;
    if (with_ssl_) {
        switch (type) {
        case websocket::MessageType::TEXT:
            std::cout << "send wss text" << std::endl;
            client_ssl_->send(connection_handle_,
                              std::move(message),
                              websocketpp::frame::opcode::text,
                              ws_error);
            break;
        case websocket::MessageType::BINARY:
            std::cout << "send wss binary" << std::endl;
            client_ssl_->send(connection_handle_,
                              std::move(message),
                              websocketpp::frame::opcode::binary,
                              ws_error);
            break;
        default: break;
        }
    }
    else if (!with_ssl_) {
        switch (type) {
        case websocket::MessageType::TEXT:
            std::cout << "send ws text" << std::endl;
            client_no_ssl_->send(connection_handle_,
                                 std::move(message),
                                 websocketpp::frame::opcode::text,
                                 ws_error);
            break;
        case websocket::MessageType::BINARY:
            std::cout << "send ws binary" << std::endl;
            client_no_ssl_->send(connection_handle_,
                                 std::move(message),
                                 websocketpp::frame::opcode::binary,
                                 ws_error);
            break;
        default: break;
        }
    }
    std::cout << "sent" << std::endl;
    if (ws_error) {
        // TODO: error handling?
    }
}

void WsClient::send(const websocket::MessageType& type, std::string&& message) {
    if (!connected_) {
        return;
    }
    std::error_code ws_error;
    if (with_ssl_) {
        switch (type) {
        case websocket::MessageType::TEXT:
            client_ssl_->send(connection_handle_,
                              message,
                              websocketpp::frame::opcode::text,
                              ws_error);
            break;
        case websocket::MessageType::BINARY:
            client_ssl_->send(connection_handle_,
                              message,
                              websocketpp::frame::opcode::binary,
                              ws_error);
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
                                 ws_error);
            break;
        case websocket::MessageType::BINARY:
            client_no_ssl_->send(connection_handle_,
                                 message,
                                 websocketpp::frame::opcode::binary,
                                 ws_error);
            break;
        default: break;
        }
    }
    if (ws_error) {
        // TODO: error handling?
    }
}

bool WsClient::connected() const {
    return connected_;
}

void WsClient::release_() {
    // release websocket client
    if (with_ssl_ && client_ssl_) {
        if (connected_) {
            client_ssl_->close(
                connection_handle_, websocketpp::close::status::normal, "bye");
        }
        client_ssl_->stop_perpetual();
        client_ssl_.reset();
    }
    else if (!with_ssl_ && client_no_ssl_) {
        if (connected_) {
            client_no_ssl_->close(
                connection_handle_, websocketpp::close::status::normal, "bye");
        }
        client_no_ssl_->stop_perpetual();
        client_no_ssl_.reset();
    }
    // release work thread
    if (work_thread_) {
        if (work_thread_->joinable()) {
            std::cout << "join" << std::endl;
            work_thread_->join();
            std::cout << "done" << std::endl;
        }
        work_thread_.reset();
    }
    connected_ = false;
}

void WsClient::connect_() {
    if (url_[2] == 's') {
        std::cout << "connect wss" << std::endl;
        // wss

        with_ssl_ = true;
        client_ssl_ = std::make_shared<
            websocketpp::client<websocketpp::config::asio_tls_client>>();
        std::error_code ws_error;
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
            });

        // TODO: should I set a fail handler?
        /* client_ssl_->set_fail_handler( */
        /*     [&](websocketpp::connection_hdl handle) -> void { */
        /*         websocketpp::client<websocketpp::config::asio_tls_client>:: */
        /*             connection_ptr connection = */
        /*                 client_ssl_->get_con_from_hdl(handle); */
        /*     }); */

        client_ssl_->set_close_handler(
            [&](websocketpp::connection_hdl handle) -> void {
                /* websocketpp::client<websocketpp::config::asio_tls_client>:: */
                /*     connection_ptr connection = */
                /*         client_ssl_->get_con_from_hdl(handle); */
                connected_ = false;
                // TODO: what should I do here?
            });

        client_ssl_->set_message_handler(
            [&](websocketpp::connection_hdl handle,
                websocketpp::client<websocketpp::config::asio_tls_client>::
                    message_ptr message) -> void {
                std::cout << "receive" << std::endl;
                switch (message->get_opcode()) {
                case websocketpp::frame::opcode::text:
                    receive_callback_(websocket::MessageType::TEXT,
                                      message->get_payload());
                    break;
                case websocketpp::frame::opcode::binary:
                    receive_callback_(websocket::MessageType::BINARY,
                                      message->get_payload());
                    break;
                default: break;
                }
                std::cout << "received" << std::endl;
            });

        client_ssl_->start_perpetual();
        work_thread_ = std::make_shared<std::thread>(
            &websocketpp::client<websocketpp::config::asio_tls_client>::run,
            client_ssl_);

        std::cout << "get connection" << std::endl;
        websocketpp::client<
            websocketpp::config::asio_tls_client>::connection_ptr connection =
            client_ssl_->get_connection(url_, ws_error);
        if (ws_error) {
            // TODO: error handling
        }

        connection_handle_ = connection->get_handle();

        std::cout << "connect" << std::endl;
        client_ssl_->connect(connection);
    }
    else {
        std::cout << "connect ws" << std::endl;
        // ws

        with_ssl_ = false;
        client_no_ssl_ = std::make_shared<
            websocketpp::client<websocketpp::config::asio_client>>();
        std::error_code ws_error;
        client_no_ssl_->clear_access_channels(websocketpp::log::alevel::all);
        client_no_ssl_->clear_error_channels(websocketpp::log::elevel::all);
        client_no_ssl_->init_asio();

        client_no_ssl_->set_open_handler(
            [&](websocketpp::connection_hdl handle) -> void {
                connected_ = true;
            });

        // TODO: should I set a fail handler?
        /* client_no_ssl_->set_fail_handler( */
        /*     [&](websocketpp::connection_hdl handle) -> void { */
        /*         websocketpp::client<websocketpp::config::asio_client>:: */
        /*             connection_ptr connection = */
        /*                 client_no_ssl_->get_con_from_hdl(handle); */
        /*     }); */
        client_no_ssl_->set_close_handler(
            [&](websocketpp::connection_hdl handle) -> void {
                /* websocketpp::client<websocketpp::config::asio_client>:: */
                /*     connection_ptr connection = */
                /*         client_no_ssl_->get_con_from_hdl(handle); */
                connected_ = false;
                // TODO: what should I do here?
            });

        client_no_ssl_->set_message_handler(
            [&](websocketpp::connection_hdl handle,
                websocketpp::client<websocketpp::config::asio_client>::
                    message_ptr message) -> void {
                std::cout << "receive" << std::endl;
                switch (message->get_opcode()) {
                case websocketpp::frame::opcode::text:
                    receive_callback_(websocket::MessageType::TEXT,
                                      message->get_payload());
                    break;
                case websocketpp::frame::opcode::binary:
                    receive_callback_(websocket::MessageType::BINARY,
                                      message->get_payload());
                    break;
                default: break;
                }
                std::cout << "received" << std::endl;
            });

        client_no_ssl_->start_perpetual();
        work_thread_ = std::make_shared<std::thread>(
            &websocketpp::client<websocketpp::config::asio_client>::run,
            client_no_ssl_);

        // get connection
        std::cout << "get connection" << std::endl;
        websocketpp::client<websocketpp::config::asio_client>::connection_ptr
            connection = client_no_ssl_->get_connection(url_, ws_error);
        if (ws_error) {
            // TODO: error handling
        }

        connection_handle_ = connection->get_handle();

        std::cout << "connect" << std::endl;
        client_no_ssl_->connect(connection);
    }
    std::cout << "connected" << std::endl;
}

} // namespace utils
} // namespace rayalto
