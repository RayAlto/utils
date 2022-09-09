#include "rautils/network/websocket/server.h"

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "libwebsockets.h"

#include "rautils/misc/atomic_queue.h"
#include "rautils/network/general/cookie.h"
#include "rautils/network/general/header.h"
#include "rautils/network/general/url.h"
#include "rautils/network/websocket/close_status.h"
#include "rautils/network/websocket/message.h"

namespace rayalto::utils::network::websocket {

constexpr const char* LWS_SERVER_DEFAULT_PROTOCOL_NAME = "";

std::string lws_peer_ip(lws* wsi) {
    char buffer[64] {};
    return lws_get_peer_simple(wsi, buffer, 64);
}

int lws_server_callback(lws* wsi,
                        lws_callback_reasons reason,
                        void* user,
                        void* in,
                        std::size_t len);

struct LwsServerCustomHeaderContext {
    std::vector<char>& buf;
    int& result_len;
    Server::ServerImpl& server_impl;
};

struct PerSessionData {
    Server::SessionWrapper* session_wrapper {nullptr};
};

class Server::SessionWrapper {
public:
    SessionWrapper(const std::string& ip_str, lws* ws_instance) :
        session(*this), ip(ip_str), wsi(ws_instance) {}
    SessionWrapper(std::string&& ip_str, lws* ws_instance) :
        session(*this), ip(std::move(ip_str)), wsi(ws_instance) {}

    /* expose to user */
    Session session;

    /* some info */
    std::string ip;

    /* used to erase current session in callback */
    std::list<SessionWrapper>::iterator iter;

    /* used to notify lws when user want to stop current session */
    lws* wsi;

    /* core (per session) */
    std::mutex wake_wsi_;
    std::atomic<bool> new_message_ = false;
    std::atomic<bool> interrupted_ = false;
    std::atomic<bool> stopped_ = true;
    std::condition_variable stopped_notofication_;
    misc::AtomicQueue<Message> message_queue_;

    /* configuration (per session) */
    std::unique_ptr<std::uint16_t> local_close_status_ = nullptr;
    std::unique_ptr<std::string> local_close_message_ = nullptr;

    /* message from client */
    std::unique_ptr<const general::Header> client_header_ = nullptr;
    std::unique_ptr<Message> receive_message_ = nullptr;
    std::unique_ptr<std::uint16_t> close_status_ = nullptr;
    std::unique_ptr<std::string> close_message_ = nullptr;
};

class Server::ServerImpl {
public:
    friend int lws_server_callback(lws* wsi,
                                   lws_callback_reasons reason,
                                   void* user,
                                   void* in,
                                   std::size_t len);

    explicit ServerImpl(Server& server);
    ServerImpl() = delete;

    // Impl does not need these
    ServerImpl(const ServerImpl&) = delete;
    ServerImpl(ServerImpl&&) noexcept = delete;
    ServerImpl& operator=(const ServerImpl&) = delete;
    ServerImpl& operator=(ServerImpl&&) noexcept = delete;

    virtual ~ServerImpl();

    // run server
    void start();
    void start(const general::Url& listen_on);
    void start(general::Url&& listen_on);

    // stop server
    void stop();
    void stop(const CloseStatus& close_status);
    void stop(const std::uint16_t& close_status);
    void stop(const std::string& message,
              const CloseStatus& close_status = CloseStatus {
                  CloseStatus::NORMAL});
    void stop(const std::string& message, const std::uint16_t& close_status);

    // if server was started
    [[nodiscard]] bool started() const;

    // listen_on, for example "ws://127.0.0.1:1080"
    const std::unique_ptr<std::string>& listen_on();
    void listen_on(const general::Url& listen_on);
    void listen_on(general::Url&& listen_on);
    void listen_on(std::nullptr_t);

    // websocket protocol name, sec-websocket-protocol
    const std::unique_ptr<std::string>& protocol();
    void protocol(const std::string& protocol);
    void protocol(std::string&& protocol);
    void protocol(std::nullptr_t);

    // server HTTP header
    const std::unique_ptr<general::Header>& header();
    void header(const general::Header& header);
    void header(general::Header&& header);
    void header(std::nullptr_t);

    // Cookie, equivalent to 'cookie' in http header
    const std::unique_ptr<general::Cookie>& cookie();
    void cookie(const general::Cookie& cookie);
    void cookie(general::Cookie&& cookie);
    void cookie(std::nullptr_t);

    // callback on server error
    const std::unique_ptr<ErrorCallback>& on_error();
    void on_error(const ErrorCallback& callback);
    void on_error(ErrorCallback&& callback);
    void on_error(std::nullptr_t);

    // callback on filter network
    const std::unique_ptr<FilterNetworkCallback>& on_filter_network();
    void on_filter_network(const FilterNetworkCallback& callback);
    void on_filter_network(FilterNetworkCallback&& callback);
    void on_filter_network(std::nullptr_t);

    // callback on filter protocol
    const std::unique_ptr<FilterProtocolCallback>& on_filter_protocol();
    void on_filter_protocol(const FilterProtocolCallback& callback);
    void on_filter_protocol(FilterNetworkCallback&& callback);
    void on_filter_protocol(std::nullptr_t);

    // callback on establish
    const std::unique_ptr<EstablishCallback>& on_establish();
    void on_establish(const EstablishCallback& callback);
    void on_establish(EstablishCallback&& callback);
    void on_establish(std::nullptr_t);

    // callback on receive
    const std::unique_ptr<ReceiveCallback>& on_receive();
    void on_receive(const ReceiveCallback& callback);
    void on_receive(ReceiveCallback&& callback);
    void on_receive(std::nullptr_t);

    // callback on session closure
    const std::unique_ptr<CloseCallback>& on_close();
    void on_close(const CloseCallback& callback);
    void on_close(CloseCallback&& callback);
    void on_close(std::nullptr_t);

    // send message to all alive sessions
    void send(const Message& message);
    void send(Message&& message);

protected:
    /* libwebsockets stuff */
    lws_context* ws_context_ = nullptr;
    lws_context_creation_info ws_context_info_ {};
    // TODO(rayalto):  set protocol
    lws_protocols ws_protocols_[2] {{LWS_SERVER_DEFAULT_PROTOCOL_NAME,
                                     lws_server_callback,
                                     sizeof(PerSessionData),
                                     0,
                                     0,
                                     nullptr,
                                     0},
                                    LWS_PROTOCOL_LIST_TERM};

    /* core (server) */
    std::atomic<bool> interrupted_ = false;
    std::atomic<bool> stopped_ = true;
    std::unique_ptr<std::thread> work_thread_ = nullptr;
    std::list<SessionWrapper> sessions_;
    std::mutex sessions_lock_;

    /* configuration (server) */
    std::unique_ptr<general::Url> listen_on_ = nullptr;
    std::unique_ptr<std::string> protocol_ = nullptr;
    std::unique_ptr<general::Header> header_ = nullptr;
    std::unique_ptr<general::Cookie> cookie_ = nullptr;
    std::unique_ptr<std::uint16_t> server_close_status_ = nullptr;
    std::unique_ptr<std::string> server_close_message_ = nullptr;

    /* callback */
    Server& server_;
    std::unique_ptr<ErrorCallback> on_error_ = nullptr;
    std::unique_ptr<FilterNetworkCallback> on_filter_network_ = nullptr;
    std::unique_ptr<FilterProtocolCallback> on_filter_protocol_ = nullptr;
    std::unique_ptr<EstablishCallback> on_establish_ = nullptr;
    std::unique_ptr<ReceiveCallback> on_receive_ = nullptr;
    std::unique_ptr<CloseCallback> on_close_ = nullptr;
};

Server::ServerImpl::ServerImpl(Server& server) : server_(server) {
    // shut the FUCK up
    lws_set_log_level(0, lwsl_emit_syslog);

    ws_context_info_.protocols = ws_protocols_;
    // pass the this pointer to lws_callback
    ws_context_info_.user = this;
}

Server::ServerImpl::~ServerImpl() {
    std::list<SessionWrapper>::iterator session_iter;
    for (;;) {
        std::unique_lock<std::mutex> lock(sessions_lock_);
        session_iter = sessions_.begin();
        if (sessions_.empty()) {
            break;
        }
        session_iter->interrupted_ = true;
        lws_callback_on_writable(session_iter->wsi);
        while (!session_iter->stopped_) {
            session_iter->stopped_notofication_.wait(lock);
        }
    }
    if (work_thread_ != nullptr && work_thread_->joinable()) {
        work_thread_->detach();
    }
    if (ws_context_ != nullptr) {
        lws_context_destroy(ws_context_);
    }
}

void Server::ServerImpl::start() {
    if (listen_on_ != nullptr) {
        if (listen_on_->host() != nullptr) {
            ws_context_info_.iface = listen_on_->host()->c_str();
        }

        if (listen_on_->port() != nullptr) {
            ws_context_info_.port = *(listen_on_->port());
        }
    }
    // TODO(rayalto): handle ssl/tls
    /* ws_context_info_.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT; */
    ws_context_info_.options = 0;
    // TODO(rayalto): handle protocol
}

void Server::ServerImpl::start(const general::Url& listen_on) {
    listen_on_ = std::make_unique<general::Url>(listen_on);
}

void Server::ServerImpl::start(general::Url&& listen_on) {
    listen_on_ = std::make_unique<general::Url>(std::move(listen_on));
}

void Server::ServerImpl::stop() {
    // ...
}

void Server::ServerImpl::stop(const CloseStatus& close_status) {
    // ...
}

void Server::ServerImpl::stop(const std::uint16_t& close_status) {
    // ...
}

void Server::ServerImpl::stop(const std::string& message,
                              const CloseStatus& close_status) {
    // ...
}

void Server::ServerImpl::stop(const std::string& message,
                              const std::uint16_t& close_status) {
    // ...
}

[[nodiscard]] bool Server::ServerImpl::started() const {
    // ...
}

const std::unique_ptr<std::string>& Server::ServerImpl::listen_on() {
    // ...
}

void Server::ServerImpl::listen_on(const general::Url& listen_on) {
    // ...
}

void Server::ServerImpl::listen_on(general::Url&& listen_on) {
    // ...
}

void Server::ServerImpl::listen_on(std::nullptr_t) {
    // ...
}

const std::unique_ptr<std::string>& Server::ServerImpl::protocol() {
    // ...
}

void Server::ServerImpl::protocol(const std::string& protocol) {
    // ...
}

void Server::ServerImpl::protocol(std::string&& protocol) {
    // ...
}

void Server::ServerImpl::protocol(std::nullptr_t) {
    // ...
}

const std::unique_ptr<general::Header>& Server::ServerImpl::header() {
    // ...
}

void Server::ServerImpl::header(const general::Header& header) {
    // ...
}

void Server::ServerImpl::header(general::Header&& header) {
    // ...
}

void Server::ServerImpl::header(std::nullptr_t) {
    // ...
}

const std::unique_ptr<general::Cookie>& Server::ServerImpl::cookie() {
    // ...
}

void Server::ServerImpl::cookie(const general::Cookie& cookie) {
    // ...
}

void Server::ServerImpl::cookie(general::Cookie&& cookie) {
    // ...
}

void Server::ServerImpl::cookie(std::nullptr_t) {
    // ...
}

const std::unique_ptr<Server::ErrorCallback>& Server::ServerImpl::on_error() {
    // ...
}

void Server::ServerImpl::on_error(const ErrorCallback& callback) {
    // ...
}

void Server::ServerImpl::on_error(ErrorCallback&& callback) {
    // ...
}

void Server::ServerImpl::on_error(std::nullptr_t) {
    // ...
}

const std::unique_ptr<Server::FilterNetworkCallback>&
Server::ServerImpl::on_filter_network() {
    // ...
}

void Server::ServerImpl::on_filter_network(
    const FilterNetworkCallback& callback) {
    // ...
}

void Server::ServerImpl::on_filter_network(FilterNetworkCallback&& callback) {
    // ...
}

void Server::ServerImpl::on_filter_network(std::nullptr_t) {
    // ...
}

const std::unique_ptr<Server::FilterProtocolCallback>&
Server::ServerImpl::on_filter_protocol() {
    // ...
}

void Server::ServerImpl::on_filter_protocol(
    const FilterProtocolCallback& callback) {
    // ...
}

void Server::ServerImpl::on_filter_protocol(FilterNetworkCallback&& callback) {
    // ...
}

void Server::ServerImpl::on_filter_protocol(std::nullptr_t) {
    // ...
}

const std::unique_ptr<Server::EstablishCallback>&
Server::ServerImpl::on_establish() {
    // ...
}

void Server::ServerImpl::on_establish(const EstablishCallback& callback) {
    // ...
}

void Server::ServerImpl::on_establish(EstablishCallback&& callback) {
    // ...
}

void Server::ServerImpl::on_establish(std::nullptr_t) {
    // ...
}

const std::unique_ptr<Server::ReceiveCallback>&
Server::ServerImpl::on_receive() {
    // ...
}

void Server::ServerImpl::on_receive(const ReceiveCallback& callback) {
    // ...
}

void Server::ServerImpl::on_receive(ReceiveCallback&& callback) {
    // ...
}

void Server::ServerImpl::on_receive(std::nullptr_t) {
    // ...
}

const std::unique_ptr<Server::CloseCallback>& Server::ServerImpl::on_close() {
    // ...
}

void Server::ServerImpl::on_close(const CloseCallback& callback) {
    // ...
}

void Server::ServerImpl::on_close(CloseCallback&& callback) {
    // ...
}

void Server::ServerImpl::on_close(std::nullptr_t) {
    // ...
}

void Server::ServerImpl::send(const Message& message) {
    // ...
}

void Server::ServerImpl::send(Message&& message) {
    // ...
}

Server::Server() {
    impl_ = std::make_unique<ServerImpl>(*this);
}

Server::Server(Server&&) noexcept = default;
Server& Server::operator=(Server&&) noexcept = default;
Server::~Server() = default;

} // namespace rayalto::utils::network::websocket
