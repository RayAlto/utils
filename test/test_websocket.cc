#include <chrono>
#include <iostream>
#include <thread>

#include "websocket/ws_client.h"

using rayalto::utils::websocket::MessageType;
using rayalto::utils::WsClient;

int main(int argc, char const* argv[]) {
    WsClient client;
    client.on_receive(
        [&](const MessageType& type, const std::string& message) -> void {
            std::cout << message << std::endl;
        });
    client.url("ws://127.0.0.1:8080");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    client.send(MessageType::TEXT, "hello");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // my echo server with tls, DO NOT mess around
    client.url("wss://test.rayalto.top:8443");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    client.send(MessageType::TEXT, "hello");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}
