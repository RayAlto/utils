#include <chrono>
#include <iostream>
#include <thread>

#include <websocket/ws_client.h>

using rayalto::utils::WsClient;

int main(int argc, char const *argv[]) {
    WsClient client;
    client.remote_address("127.0.0.1");
    client.remote_path("/foo");
    client.remote_port(8080);
    client.header_host("127.0.0.1");
    client.header_origin("127.0.0.1");
    auto foo = client.run();
    std::this_thread::sleep_for(std::chrono::seconds(1));
        client.send("hello");
    std::this_thread::sleep_for(std::chrono::seconds(100));
    return 0;
}
