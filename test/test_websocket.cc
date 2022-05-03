#include <chrono>
#include <iostream>
#include <system_error>
#include <thread>

#include "websocket/close_status.h"
#include "websocket/ws_client.h"

using namespace rayalto::utils;
using websocket::CloseStatus;
using websocket::MessageType;

int main(int argc, char const* argv[]) {
    // error
    std::error_code ws_error;

    // default close reason
    WsClient::default_close_status().reason = "shut up";
    WsClient client;

    client.on_receive([&](WsClient& client,
                          const MessageType& type,
                          const std::string& message) -> void {
        std::cout << "receive: " << message << std::endl;
    });

    // connection established
    client.on_establish([&](WsClient& client) -> void {
        std::cout << "connected: " << client.url() << std::endl;
    });

    // connection failed
    client.on_fail(
        [&](WsClient& client, const CloseStatus& local_status) -> void {
            std::cout << "failed to connect (" << local_status.code
                      << "): " << local_status.status << std::endl;
        });

    // connection closed
    client.on_close(
        [&](WsClient& client, const CloseStatus& remote_status) -> void {
            std::cout << "closed (" << remote_status.code
                      << "): " << remote_status.status << ", "
                      << remote_status.reason << std::endl;
        });

    // local echo server
    client.url("ws://127.0.0.1:8080");
    client.connect(ws_error);

    if (ws_error) {
        std::cout << "error get connection: " << ws_error.message()
                  << std::endl;
    }

    else {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // say hello
        client.send(MessageType::TEXT, "hello", ws_error);
        std::this_thread::sleep_for(std::chrono::seconds(2));

        if (ws_error) {
            std::cout << "error send message: " << ws_error.message()
                      << std::endl;
        }

        // fuck off
        client.disconnect(CloseStatus(CloseStatus::Code::NORMAL, "fuck off"));
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // my echo server with tls, DO NOT mess around
    client.url("wss://test.rayalto.top:8443");
    client.connect(ws_error);

    if (ws_error) {
        std::cout << "error get connection: " << ws_error.message()
                  << std::endl;
    }

    else {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // say hello
        client.send(MessageType::TEXT, "hello", ws_error);

        if (ws_error) {
            std::cout << "error send message: " << ws_error.message()
                      << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    return 0;
    // did not call disconnect, default status will be sent
}
