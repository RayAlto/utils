#ifndef RA_UTILS_WEBSOCKET_WS_CLIENT_H_
#define RA_UTILS_WEBSOCKET_WS_CLIENT_H_

#include <cstddef>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

#include "libwebsockets.h"

namespace rayalto {
namespace utils {

class WsClient {
public:
    WsClient();
    WsClient(const WsClient&) = delete;
    WsClient(WsClient&&) noexcept = delete;
    WsClient& operator=(const WsClient&) = delete;
    WsClient& operator=(WsClient&&) noexcept = delete;

    virtual ~WsClient();

    std::pair<bool, std::string> run();
    void stop();

    const bool& running() const;

    const std::string& received_message() const;

    int& remote_port();
    const int& remote_port() const;
    void remote_port(const int& port);
    void remote_port(int&& port);

    std::string& remote_address();
    const std::string& remote_address() const;
    void remote_address(const std::string& address);
    void remote_address(std::string&& address);

    std::string& remote_path();
    const std::string& remote_path() const;
    void remote_path(const std::string& path);
    void remote_path(std::string&& path);

    std::string& header_host();
    const std::string& header_host() const;
    void header_host(const std::string& host);
    void header_host(std::string&& host);

    std::string& header_origin();
    const std::string& header_origin() const;
    void header_origin(const std::string& origin);
    void header_origin(std::string&& origin);

    std::string& local_interface();
    const std::string& local_interface() const;
    void local_interface(const std::string& interface);
    void local_interface(std::string&& interface);

    void send(const std::string& message);
    void send(std::string&& message);

protected:
    bool running_ = false;
    bool closed_ = false;
    std::string received_message_;
    std::string message_to_send_;
    int remote_port_ = 443;
    std::string remote_address_;
    std::string remote_path_;
    std::string header_host_;
    std::string header_origin_;
    std::string local_interface_;

    std::thread work_thread_;
    std::mutex received_message_mutex_;
    std::mutex message_to_send_mutex_;
    std::mutex closed_mutex_;

    lws_context* websocket_context_ = nullptr;
    lws_context_creation_info context_info_ {0};
    lws_client_connect_info connect_info_ {0};
    lws* websocket_ = nullptr;

    lws_protocols websocket_protocols[2];

    friend int ws_callback(lws* wsi,
                           lws_callback_reasons reason,
                           void* user,
                           void* in,
                           std::size_t len);
};

} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_WEBSOCKET_WS_CLIENT_H_
