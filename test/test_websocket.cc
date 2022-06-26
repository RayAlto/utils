#include <chrono>
#include <iostream>
#include <thread>

#include "rautils/network/general/url.h"
#include "rautils/network/websocket.h"
#include "rautils/network/websocket/close_status.h"
#include "rautils/network/websocket/message.h"
#include "rautils/string/strtool.h"

using rayalto::utils::network::websocket::Client;
using rayalto::utils::network::websocket::Message;
using rayalto::utils::network::general::Url;
using rayalto::utils::network::websocket::CloseStatus;

int main(int argc, char const* argv[]) {
    Client client;
    client.on_error([&](Client& client, const std::string& message) -> void {
        std::cerr << "Error: " << message << std::endl;
    });
    client.on_close([&](Client& client,
                        const CloseStatus& close_status,
                        const std::string& message) -> void {
        std::cout << "Closed(" << close_status.value() << "): " << message
                  << std::endl;
    });
    client.on_establish([&](Client& client) -> void {
        std::cout << "Connected: " << client.url()->to_string() << std::endl;
    });
    client.on_receive([&](Client& client, const Message& message) -> void {
        std::cout << "Message from " << client.url()->to_string() << " : "
                  << (message.type() == Message::Type::TEXT
                          ? message.text()
                          : rayalto::utils::string::hex_string(
                              message.binary()))
                  << std::endl;
    });
    client.connect(Url("wss://test.rayalto.top:8443"));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    client.send(Message("hello world"));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    client.disconnect("bye", CloseStatus::NORMAL);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    client.connect(Url("ws://127.0.0.1:8080"));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    client.send(Message("hello world"));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    client.disconnect("bye", CloseStatus::NORMAL);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
