#include "websocket/ws_client.h"

#include <cstddef>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

#include "libwebsockets.h"

namespace rayalto {
namespace utils {

int ws_callback(lws* wsi,
                lws_callback_reasons reason,
                void* user,
                void* in,
                std::size_t len) {
    WsClient* client =
        reinterpret_cast<WsClient*>(lws_context_user(lws_get_context(wsi)));
    switch (reason) {
    case LWS_CALLBACK_CLIENT_WRITEABLE: {
        std::unique_lock<std::mutex> send_lock(client->message_to_send_mutex_);
        if (!client->message_to_send_.empty()) {
            std::size_t data_length = client->message_to_send_.length();
            char data[LWS_PRE + data_length];
            client->message_to_send_.copy(&data[LWS_PRE], data_length);
            client->message_to_send_.clear();
            lws_write(wsi,
                      reinterpret_cast<unsigned char*>(&data[LWS_PRE]),
                      data_length,
                      LWS_WRITE_TEXT);
        }
        // TODO deal with close
        if (false) {
            lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, nullptr, 0);
            return -1;
            break;
        }
    }
    case LWS_CALLBACK_CLIENT_RECEIVE: {
        std::unique_lock<std::mutex> receive_lock(
            client->received_message_mutex_);
        client->received_message_ =
            std::string(reinterpret_cast<char*>(in), len);
        break;
    }
    case LWS_CALLBACK_CLIENT_CLOSED: {
        client->running_ = false;
        return -1;
        break;
    }
    default: break;
    }
    return 0;
}

WsClient::WsClient() {
    lws_set_log_level(0, nullptr);
    context_info_.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    context_info_.port = CONTEXT_PORT_NO_LISTEN;
}

WsClient::~WsClient() {
    // ...
}

std::pair<bool, std::string> WsClient::run() {
    if (running_) {
        return {false, "alread running"};
    }
    running_ = true;
    // context info
    websocket_protocols[0] = {
        "ra-utils-wsclient", ws_callback, 0, 0, 0, nullptr, 0};
    websocket_protocols[1] = LWS_PROTOCOL_LIST_TERM;
    context_info_.protocols = websocket_protocols;
    context_info_.user = this;
    // create context
    websocket_context_ = lws_create_context(&context_info_);
    if (!websocket_context_) {
        return {false, "failed to create context"};
    }
    // connect info
    connect_info_.context = websocket_context_;
    connect_info_.port = remote_port_;
    connect_info_.address = remote_address_.c_str();
    connect_info_.path = remote_path_.c_str();
    connect_info_.host = header_host_.c_str();
    connect_info_.origin = header_origin_.c_str();
    connect_info_.iface = local_interface_.c_str();
    connect_info_.protocol = "ra-utils-wsclient";
    // connect to server
    websocket_ = lws_client_connect_via_info(&connect_info_);
    if (!websocket_) {
        return {false, "failed to connect to server"};
    }
    // work thread
    work_thread_ = std::thread([&]() {
        while (lws_service(websocket_context_, 0) >= 0 && running_) {
            lws_callback_on_writable(websocket_);
        }
        lws_context_destroy(websocket_context_);
        websocket_context_ = nullptr;
        websocket_ = nullptr;
        running_ = false;
    });
    work_thread_.detach();
    return {true, std::string {}};
}

void WsClient::stop() {
    // ...
}

const bool& WsClient::running() const {
    return running_;
}

const std::string& WsClient::received_message() const {
    return received_message_;
}

int& WsClient::remote_port() {
    return remote_port_;
}

const int& WsClient::remote_port() const {
    return remote_port_;
}

void WsClient::remote_port(const int& port) {
    remote_port_ = port;
}

void WsClient::remote_port(int&& port) {
    remote_port_ = std::move(port);
}

std::string& WsClient::remote_address() {
    return remote_address_;
}

const std::string& WsClient::remote_address() const {
    return remote_address_;
}

void WsClient::remote_address(const std::string& address) {
    remote_address_ = address;
}

void WsClient::remote_address(std::string&& address) {
    remote_address_ = std::move(address);
}

std::string& WsClient::remote_path() {
    return remote_path_;
}

const std::string& WsClient::remote_path() const {
    return remote_path_;
}

void WsClient::remote_path(const std::string& path) {
    remote_path_ = path;
}

void WsClient::remote_path(std::string&& path) {
    remote_path_ = std::move(path);
}

std::string& WsClient::header_host() {
    return header_host_;
}

const std::string& WsClient::header_host() const {
    return header_host_;
}

void WsClient::header_host(const std::string& host) {
    header_host_ = host;
}

void WsClient::header_host(std::string&& host) {
    header_host_ = std::move(host);
}

std::string& WsClient::header_origin() {
    return header_origin_;
}

const std::string& WsClient::header_origin() const {
    return header_origin_;
}

void WsClient::header_origin(const std::string& origin) {
    header_origin_ = origin;
}

void WsClient::header_origin(std::string&& origin) {
    header_origin_ = std::move(origin);
}

std::string& WsClient::local_interface() {
    return local_interface_;
}

const std::string& WsClient::local_interface() const {
    return local_interface_;
}

void WsClient::local_interface(const std::string& interface) {
    local_interface_ = interface;
}

void WsClient::local_interface(std::string&& interface) {
    local_interface_ = std::move(interface);
}

void WsClient::send(const std::string& message) {
    std::unique_lock<std::mutex> send_lock(message_to_send_mutex_);
    message_to_send_ = message;
}

void WsClient::send(std::string&& message) {
    std::unique_lock<std::mutex> send_lock(message_to_send_mutex_);
    message_to_send_ = std::move(message);
}

} // namespace utils
} // namespace rayalto
