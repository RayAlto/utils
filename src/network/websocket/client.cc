#include "rautils/network/websocket/client.h"

#include <iostream>

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "libwebsockets.h"

#include "rautils/misc/atomic_queue.h"
#include "rautils/misc/thread_id.h"
#include "rautils/network/general/cookie.h"
#include "rautils/network/general/header.h"
#include "rautils/network/general/url.h"
#include "rautils/network/websocket/close_status.h"
#include "rautils/network/websocket/message.h"

namespace rayalto {
namespace utils {
namespace network {
namespace websocket {

constexpr const char* LWS_LOCAL_PROTOCOL_NAME = "ra-utils-websocket-client";

// only for custom header callback, function pointer is fucking disgusting
struct CustomHeaderContext {
    std::vector<char>& buf;
    int& result_len;
    Client::ClientImpl& client_impl;
};

int lws_callback(lws* wsi,
                 lws_callback_reasons reason,
                 void* user,
                 void* in,
                 std::size_t len);

class Client::ClientImpl {
public:
    friend int lws_callback(lws* wsi,
                            lws_callback_reasons reason,
                            void* user,
                            void* in,
                            std::size_t len);

public:
    ClientImpl(Client& client);
    ClientImpl() = delete;

    // Impl does not need these
    ClientImpl(const ClientImpl&) = delete;
    ClientImpl(ClientImpl&&) noexcept = delete;
    ClientImpl& operator=(const ClientImpl&) = delete;
    ClientImpl& operator=(ClientImpl&&) noexcept = delete;

    virtual ~ClientImpl();

    void connect();
    void connect(const general::Url& url);
    void connect(general::Url&& url);

    void disconnect();
    void disconnect(const CloseStatus& close_status);
    void disconnect(const std::uint16_t& close_status);
    void disconnect(const std::string& message,
                    const CloseStatus& close_status = CloseStatus::NORMAL);
    void disconnect(const std::string& message,
                    const std::uint16_t& close_status);

    // url
    const std::unique_ptr<general::Url>& url();
    void url(const general::Url& url);
    void url(general::Url&& url);
    void url(std::nullptr_t);

    // websocket protocol name, sec-websocket-protocol
    const std::unique_ptr<std::string>& protocol();
    void protocol(const std::string& protocol);
    void protocol(std::string&& protocol);
    void protocol(std::nullptr_t);

    // could be name, ip address
    const std::unique_ptr<std::string>& local_interface();
    void local_interface(const std::string& interface);
    void local_interface(std::string&& interface);
    void local_interface(std::nullptr_t);

    // HTTP header
    const std::unique_ptr<general::Header>& header();
    void header(const general::Header& header);
    void header(general::Header&& header);
    void header(std::nullptr_t);

    // Cookie, equivalent to 'cookie' in http header
    const std::unique_ptr<general::Cookie>& cookie();
    void cookie(const general::Cookie& cookie);
    void cookie(general::Cookie&& cookie);
    void cookie(std::nullptr_t);

    // callback on connection error
    const std::unique_ptr<ErrorCallback>& on_error();
    void on_error(const ErrorCallback& callback);
    void on_error(ErrorCallback&& callback);
    void on_error(std::nullptr_t);

    // callback on connection establishment
    const std::unique_ptr<EstablishCallback>& on_establish();
    void on_establish(const EstablishCallback& callback);
    void on_establish(EstablishCallback&& callback);
    void on_establish(std::nullptr_t);

    // callback on receiving message
    const std::unique_ptr<ReceiveCallback>& on_receive();
    void on_receive(const ReceiveCallback& callback);
    void on_receive(ReceiveCallback&& callback);
    void on_receive(std::nullptr_t);

    // callback on connection closure
    const std::unique_ptr<CloseCallback>& on_close();
    void on_close(const CloseCallback& callback);
    void on_close(CloseCallback&& callback);
    void on_close(std::nullptr_t);

    // send message to server
    void send(const Message& message);
    void send(Message&& message);

protected:
    /* libwebsockets stuff */
    lws* ws_instance_ = nullptr;
    lws_context* ws_context_ = nullptr;
    lws_context_creation_info ws_context_info_ {0};
    lws_protocols ws_protocols_[2] {
        {LWS_LOCAL_PROTOCOL_NAME, lws_callback, 0, 0, 0, nullptr, 0},
        LWS_PROTOCOL_LIST_TERM};
    lws_client_connect_info ws_connection_info_ {0};

    /* core */
    std::mutex wake_lws_;
    std::atomic<bool> new_message_ = false;
    std::atomic<bool> interrupted_ = false;
    std::atomic<bool> stopped_ = true;
    std::unique_ptr<std::thread> work_thread_ = nullptr;
    misc::AtomicQueue<Message> message_queue_;

    /* configuration */
    std::unique_ptr<general::Url> url_ = nullptr;
    std::unique_ptr<std::string> protocol_ = nullptr;
    std::unique_ptr<std::string> local_interface_ = nullptr;
    std::unique_ptr<general::Header> header_ = nullptr;
    std::unique_ptr<general::Cookie> cookie_ = nullptr;
    std::unique_ptr<std::uint16_t> local_close_status_ = nullptr;
    std::unique_ptr<std::string> local_close_message_ = nullptr;

    /* callback */
    Client& client_;
    std::unique_ptr<ErrorCallback> on_error_ = nullptr;
    std::unique_ptr<EstablishCallback> on_establish_ = nullptr;
    std::unique_ptr<ReceiveCallback> on_receive_ = nullptr;
    std::unique_ptr<CloseCallback> on_close_ = nullptr;

    /* response from server */
    std::unique_ptr<general::Header> server_header_ = nullptr;
    std::unique_ptr<Message> receive_message_ = nullptr;
    std::unique_ptr<std::uint16_t> close_status_ = nullptr;
    std::unique_ptr<std::string> close_message = nullptr;

protected:
    void wake_lws_up_();
};

Client::ClientImpl::ClientImpl(Client& client) : client_(client) {
    // shut the FUCK up
    lws_set_log_level(0, lwsl_emit_syslog);
    // this is a client
    ws_context_info_.port = CONTEXT_PORT_NO_LISTEN;
    ws_context_info_.protocols = ws_protocols_;
    // pass the this pointer to lws_callback
    ws_context_info_.user = this;
    ws_connection_info_.local_protocol_name = LWS_LOCAL_PROTOCOL_NAME;
    ws_connection_info_.pwsi = &ws_instance_;
}

Client::ClientImpl::~ClientImpl() {
    if (!stopped_) {
        disconnect();
    }
    if (work_thread_ != nullptr && work_thread_->joinable()) {
        work_thread_->detach();
    }
    if (ws_context_ != nullptr) {
        lws_context_destroy(ws_context_);
    }
}

void Client::ClientImpl::connect() {
    if (url_ != nullptr) {
        if (url_->host() != nullptr) {
            ws_connection_info_.address = url_->host()->c_str();
            ws_connection_info_.host = url_->host()->c_str();
            std::cout << "connect address/host " << *url_->host() << std::endl;
        }

        if (url_->path() != nullptr) {
            ws_connection_info_.path = url_->path()->c_str();
            std::cout << "connect path " << *url_->path() << std::endl;
        }

        if (url_->port() != nullptr) {
            ws_connection_info_.port = *(url_->port());

            if ((url_->scheme() == nullptr && ws_connection_info_.port == 443)
                || (url_->scheme() != nullptr && *url_->scheme() == "wss")) {
                // use port 443 or scheme "wss"
                ws_context_info_.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
                ws_connection_info_.ssl_connection = LCCSCF_USE_SSL;
            }
            else {
                ws_context_info_.options = 0;
                ws_connection_info_.ssl_connection = 0;
            }

            std::cout << "connect port " << *url_->port() << std::endl;
        }
    }

    if (protocol_ != nullptr) {
        ws_connection_info_.protocol = protocol_->c_str();
    }

    ws_context_ = lws_create_context(&ws_context_info_);
    if (ws_context_ == nullptr) {
        if (on_error_ != nullptr) {
            (*on_error_)(client_, "libwebsockets: Failed to create context");
        }
        return;
    }

    ws_connection_info_.context = ws_context_;

    if (!lws_client_connect_via_info(&ws_connection_info_)) {
        if (on_error_ != nullptr) {
            (*on_error_)(client_, "libwebsockets: Failed to connect");
        }
        return;
    }

    new_message_ = !message_queue_.empty();
    interrupted_ = false;
    stopped_ = false;

    work_thread_ = std::make_unique<std::thread>([&]() -> void {
        int status = 0;
        while (status >= 0 && !stopped_) {
            status = lws_service(ws_context_, 0);
        }
        lws_context_destroy(ws_context_);
        ws_context_ = nullptr;
    });
    work_thread_->detach();
}

void Client::ClientImpl::connect(const general::Url& url) {
    url_ = std::make_unique<general::Url>(url);
    connect();
}

void Client::ClientImpl::connect(general::Url&& url) {
    url_ = std::make_unique<general::Url>(std::move(url));
    connect();
}

void Client::ClientImpl::disconnect() {
    interrupted_ = true;
    wake_lws_up_();
    while (!stopped_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    interrupted_ = false;
}

void Client::ClientImpl::disconnect(const CloseStatus& close_status) {
    local_close_status_ = std::make_unique<std::uint16_t>(close_status.value());
    disconnect();
}

void Client::ClientImpl::disconnect(const std::uint16_t& close_status) {
    local_close_status_ = std::make_unique<std::uint16_t>(close_status);
    disconnect();
}

void Client::ClientImpl::disconnect(const std::string& message,
                                    const CloseStatus& close_status) {
    local_close_message_ = std::make_unique<std::string>(message);
    local_close_status_ = std::make_unique<std::uint16_t>(close_status.value());
    disconnect();
}

void Client::ClientImpl::disconnect(const std::string& message,
                                    const std::uint16_t& close_status) {
    local_close_message_ = std::make_unique<std::string>(message);
    local_close_status_ = std::make_unique<std::uint16_t>(close_status);
    disconnect();
}

const std::unique_ptr<general::Url>& Client::ClientImpl::url() {
    return url_;
}

void Client::ClientImpl::url(const general::Url& url) {
    url_ = std::make_unique<general::Url>(url);
}

void Client::ClientImpl::url(general::Url&& url) {
    url_ = std::make_unique<general::Url>(std::move(url));
}

void Client::ClientImpl::url(std::nullptr_t) {
    url_ = nullptr;
}

const std::unique_ptr<std::string>& Client::ClientImpl::protocol() {
    return protocol_;
}

void Client::ClientImpl::protocol(const std::string& protocol) {
    protocol_ = std::make_unique<std::string>(protocol);
}

void Client::ClientImpl::protocol(std::string&& protocol) {
    protocol_ = std::make_unique<std::string>(std::move(protocol));
}

void Client::ClientImpl::protocol(std::nullptr_t) {
    protocol_ = nullptr;
}

const std::unique_ptr<std::string>& Client::ClientImpl::local_interface() {
    return local_interface_;
}

void Client::ClientImpl::local_interface(const std::string& interface) {
    local_interface_ = std::make_unique<std::string>(interface);
}

void Client::ClientImpl::local_interface(std::string&& interface) {
    local_interface_ = std::make_unique<std::string>(std::move(interface));
}

void Client::ClientImpl::local_interface(std::nullptr_t) {
    local_interface_ = nullptr;
}

const std::unique_ptr<general::Header>& Client::ClientImpl::header() {
    return header_;
}

void Client::ClientImpl::header(const general::Header& header) {
    header_ = std::make_unique<general::Header>(header);
}

void Client::ClientImpl::header(general::Header&& header) {
    header_ = std::make_unique<general::Header>(std::move(header));
}

void Client::ClientImpl::header(std::nullptr_t) {
    header_ = nullptr;
}

const std::unique_ptr<general::Cookie>& Client::ClientImpl::cookie() {
    return cookie_;
}

void Client::ClientImpl::cookie(const general::Cookie& cookie) {
    cookie_ = std::make_unique<general::Cookie>(cookie);
}

void Client::ClientImpl::cookie(general::Cookie&& cookie) {
    cookie_ = std::make_unique<general::Cookie>(std::move(cookie));
}

void Client::ClientImpl::cookie(std::nullptr_t) {
    cookie_ = nullptr;
}

const std::unique_ptr<Client::ErrorCallback>& Client::ClientImpl::on_error() {
    return on_error_;
}

void Client::ClientImpl::on_error(const ErrorCallback& callback) {
    on_error_ = std::make_unique<ErrorCallback>(callback);
}

void Client::ClientImpl::on_error(ErrorCallback&& callback) {
    on_error_ = std::make_unique<ErrorCallback>(std::move(callback));
}

void Client::ClientImpl::on_error(std::nullptr_t) {
    on_error_ = nullptr;
}

const std::unique_ptr<Client::EstablishCallback>&
Client::ClientImpl::on_establish() {
    return on_establish_;
}

void Client::ClientImpl::on_establish(const EstablishCallback& callback) {
    on_establish_ = std::make_unique<EstablishCallback>(callback);
}

void Client::ClientImpl::on_establish(EstablishCallback&& callback) {
    on_establish_ = std::make_unique<EstablishCallback>(std::move(callback));
}

void Client::ClientImpl::on_establish(std::nullptr_t) {
    on_establish_ = nullptr;
}

const std::unique_ptr<Client::ReceiveCallback>&
Client::ClientImpl::on_receive() {
    return on_receive_;
}

void Client::ClientImpl::on_receive(const ReceiveCallback& callback) {
    on_receive_ = std::make_unique<ReceiveCallback>(callback);
}

void Client::ClientImpl::on_receive(ReceiveCallback&& callback) {
    on_receive_ = std::make_unique<ReceiveCallback>(std::move(callback));
}

void Client::ClientImpl::on_receive(std::nullptr_t) {
    on_receive_ = nullptr;
}

const std::unique_ptr<Client::CloseCallback>& Client::ClientImpl::on_close() {
    return on_close_;
}

void Client::ClientImpl::on_close(const CloseCallback& callback) {
    on_close_ = std::make_unique<CloseCallback>(callback);
}

void Client::ClientImpl::on_close(CloseCallback&& callback) {
    on_close_ = std::make_unique<CloseCallback>(std::move(callback));
}

void Client::ClientImpl::on_close(std::nullptr_t) {
    on_close_ = nullptr;
}

void Client::ClientImpl::send(const Message& message) {
    message_queue_.push(message);
    new_message_ = !message_queue_.empty();
    wake_lws_up_();
}

void Client::ClientImpl::send(Message&& message) {
    message_queue_.push(std::move(message));
    new_message_ = !message_queue_.empty();
    wake_lws_up_();
}

void Client::ClientImpl::wake_lws_up_() {
    wake_lws_.lock();
    std::cout << "wake up!" << std::endl;
    lws_callback_on_writable(ws_instance_);
    wake_lws_.unlock();
}

int lws_callback(lws* wsi,
                 lws_callback_reasons reason,
                 void* user,
                 void* in,
                 std::size_t len) {
    Client::ClientImpl& client_impl = *reinterpret_cast<Client::ClientImpl*>(
        lws_context_user(lws_get_context(wsi)));

    switch (reason) {
    case /*  1 */ LWS_CALLBACK_CLIENT_CONNECTION_ERROR: {
        if (client_impl.on_error_ != nullptr) {
            (*client_impl.on_error_)(
                client_impl.client_,
                in == nullptr ? std::string {}
                              : std::string(reinterpret_cast<char*>(in)));
        }
        client_impl.stopped_ = true;
        break;
    }

    case /*  2 */ LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH: {
        if (client_impl.server_header_ == nullptr) {
            client_impl.server_header_ = std::make_unique<general::Header>();
        }
        std::vector<char> buf;
        int result_len = 0;
        int index = 0;
        while (true) {
            lws_token_indexes lws_index = static_cast<lws_token_indexes>(index);
            if (lws_index == _WSI_TOKEN_CLIENT_SENT_PROTOCOLS) {
                break;
            }
            // length of header value
            result_len = lws_hdr_total_length(wsi, lws_index);
            if (result_len > 0) {
                if (result_len > buf.capacity()) {
                    // has to allocate a larger buffer
                    buf.reserve(result_len + 4);
                }
                // real length of header value copied to buffer
                result_len =
                    lws_hdr_copy(wsi, buf.data(), buf.capacity(), lws_index);
                client_impl.server_header_->add(
                    {reinterpret_cast<const char*>(
                         lws_token_to_string(lws_index)),
                     std::string(buf.data(), result_len)});
            }
            ++index;
        }
        CustomHeaderContext custom_header_ctx {buf, result_len, client_impl};
        lws_hdr_custom_name_foreach(
            wsi,
            [](const char* name, int name_len, void* opaque) -> void {
                CustomHeaderContext& custom_header_ctx =
                    *reinterpret_cast<CustomHeaderContext*>(opaque);
                custom_header_ctx.result_len = lws_hdr_custom_copy(
                    custom_header_ctx.client_impl.ws_instance_,
                    custom_header_ctx.buf.data(),
                    custom_header_ctx.buf.capacity(),
                    name,
                    name_len);
                custom_header_ctx.client_impl.server_header_->add(
                    {std::string(name, name_len),
                     std::string(custom_header_ctx.buf.data(),
                                 custom_header_ctx.result_len)});
            },
            &custom_header_ctx);
        break;
    }

    case /*  3 */ LWS_CALLBACK_CLIENT_ESTABLISHED: {
        if (client_impl.on_establish_ != nullptr) {
            (*client_impl.on_establish_)(client_impl.client_);
        }
        break;
    }

    case /*  8 */ LWS_CALLBACK_CLIENT_RECEIVE: {
        if (lws_is_first_fragment(wsi)) {
            if (lws_frame_is_binary(wsi)) {
                unsigned char* binary_message =
                    reinterpret_cast<unsigned char*>(in);
                client_impl.receive_message_ =
                    std::make_unique<Message>(std::vector<unsigned char>(
                        binary_message, binary_message + len));
            }
            else {
                client_impl.receive_message_ = std::make_unique<Message>(
                    std::string(reinterpret_cast<char*>(in), len));
            }
            if (lws_is_final_fragment(wsi)) {
                if (client_impl.on_receive_ != nullptr) {
                    (*client_impl.on_receive_)(client_impl.client_,
                                               *client_impl.receive_message_);
                }
            }
            else {
                client_impl.receive_message_->reserve(
                    len + lws_remaining_packet_payload(wsi) + 4);
            }
            break;
        }
        else {
            switch (client_impl.receive_message_->type()) {
            case Message::Type::BINARY: {
                unsigned char* binary_message =
                    reinterpret_cast<unsigned char*>(in);
                std::vector<unsigned char>& binary =
                    client_impl.receive_message_->binary();
                binary.insert(
                    binary.end(), binary_message, binary_message + len);
                break;
            }
            case Message::Type::TEXT: {
                client_impl.receive_message_->text().append(
                    reinterpret_cast<char*>(in), len);
                break;
            }
            default: break;
            }
            if (lws_is_final_fragment(wsi)) {
                if (client_impl.on_receive_ != nullptr) {
                    (*client_impl.on_receive_)(client_impl.client_,
                                               *client_impl.receive_message_);
                }
            }
            break;
        }
        break;
    }

    case /* 10 */ LWS_CALLBACK_CLIENT_WRITEABLE: {
        std::cout << "callback client writable" << std::endl;
        if (client_impl.interrupted_) {
            const std::unique_ptr<std::uint16_t>& status =
                client_impl.local_close_status_;
            if (status == nullptr) {
                // close status is not specified, just close
                return -1;
            }

            const std::unique_ptr<std::string>& message =
                client_impl.local_close_message_;
            unsigned char* close_message = nullptr;
            std::size_t close_message_length = 0;
            if (message != nullptr) {
                close_message =
                    reinterpret_cast<unsigned char*>(message->data());
                close_message_length = message->length();
            }
            lws_close_reason(wsi,
                             static_cast<lws_close_status>(*status),
                             close_message,
                             close_message_length);
            return -1;
        }
        if (client_impl.new_message_) {
            Message& message = client_impl.message_queue_.front();
            switch (message.type()) {
            case Message::Type::BINARY:
                lws_write(wsi,
                          reinterpret_cast<unsigned char*>(message.data()),
                          message.length(),
                          LWS_WRITE_BINARY);
                break;
            case Message::Type::TEXT:
                lws_write(wsi,
                          reinterpret_cast<unsigned char*>(message.data()),
                          message.length(),
                          LWS_WRITE_TEXT);
                break;
            default: break;
            }
            client_impl.message_queue_.pop();
            client_impl.new_message_ = !client_impl.message_queue_.empty();
            if (client_impl.new_message_) {
                lws_callback_on_writable(wsi);
            }
        }
        break;
    }

    case /* 24 */ LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER: {
        if (client_impl.header_ == nullptr) {
            break;
        }
        unsigned char** p = reinterpret_cast<unsigned char**>(in);
        unsigned char* end = (*p) + len;
        for (const std::pair<std::string, std::string>& header :
             *client_impl.header_) {
            const std::size_t name_length = header.first.length();
            char name[name_length + 2];
            header.first.copy(name, name_length);
            name[name_length] = ':';
            name[name_length + 1] = '\0';
            if (lws_add_http_header_by_name(
                    wsi,
                    reinterpret_cast<const unsigned char*>(name),
                    reinterpret_cast<const unsigned char*>(
                        header.second.c_str()),
                    header.second.length(),
                    p,
                    end)) {
                // what should I do
            }
        }
        break;
    }

    case /* 31 */ LWS_CALLBACK_GET_THREAD_ID: {
        std::cout << "callback on thread id" << std::endl;
        return misc::thread_id();
        break;
    }

    case /* 38 */ LWS_CALLBACK_WS_PEER_INITIATED_CLOSE: {
        const std::uint8_t* message_bytes = reinterpret_cast<std::uint8_t*>(in);
        std::size_t message_length = len - 2;
        client_impl.close_status_ = std::make_unique<std::uint16_t>(
            (message_bytes[0] << 8) | message_bytes[1]);
        client_impl.close_message = std::make_unique<std::string>(
            &reinterpret_cast<char*>(in)[2], len - 2);
        break;
    }

    case /* 75 */ LWS_CALLBACK_CLIENT_CLOSED: {
        if (client_impl.on_close_ != nullptr) {
            if (client_impl.close_status_ == nullptr) {
                (*client_impl.on_close_)(
                    client_impl.client_, CloseStatus::ABNORMAL, std::string {});
            }
            else {
                (*client_impl.on_close_)(client_impl.client_,
                                         *client_impl.close_status_,
                                         *client_impl.close_message);
            }
        }
        client_impl.stopped_ = true;
        break;
    }

    default: break;
    }

    return 0;
}

Client::Client() {
    impl_ = std::make_unique<ClientImpl>(*this);
}

Client::Client(Client&&) noexcept = default;
Client& Client::operator=(Client&&) noexcept = default;
Client::~Client() = default;

Client& Client::connect() {
    impl_->connect();
    return *this;
}

Client& Client::connect(const general::Url& url) {
    impl_->connect(url);
    return *this;
}

Client& Client::connect(general::Url&& url) {
    impl_->connect(std::move(url));
    return *this;
}

Client& Client::disconnect() {
    impl_->disconnect();
    return *this;
}

Client& Client::disconnect(const CloseStatus& close_status) {
    impl_->disconnect(close_status);
    return *this;
}

Client& Client::disconnect(const std::uint16_t& close_status) {
    impl_->disconnect(close_status);
    return *this;
}

Client& Client::disconnect(const std::string& message,
                           const CloseStatus& close_status) {
    impl_->disconnect(message, close_status);
    return *this;
}

Client& Client::disconnect(const std::string& message,
                           const std::uint16_t& close_status) {
    impl_->disconnect(message, close_status);
    return *this;
}

const std::unique_ptr<general::Url>& Client::url() {
    return impl_->url();
}

Client& Client::url(const general::Url& url) {
    impl_->url(url);
    return *this;
}

Client& Client::url(general::Url&& url) {
    impl_->url(std::move(url));
    return *this;
}

Client& Client::url(std::nullptr_t) {
    impl_->url(nullptr);
    return *this;
}

const std::unique_ptr<std::string>& Client::protocol() {
    return impl_->protocol();
}

Client& Client::protocol(const std::string& protocol) {
    impl_->protocol(protocol);
    return *this;
}

Client& Client::protocol(std::string&& protocol) {
    impl_->protocol(std::move(protocol));
    return *this;
}

Client& Client::protocol(std::nullptr_t) {
    impl_->protocol(nullptr);
    return *this;
}

const std::unique_ptr<std::string>& Client::local_interface() {
    return impl_->local_interface();
}

Client& Client::local_interface(const std::string& interface) {
    impl_->local_interface(interface);
    return *this;
}

Client& Client::local_interface(std::string&& interface) {
    impl_->local_interface(std::move(interface));
    return *this;
}

Client& Client::local_interface(std::nullptr_t) {
    impl_->local_interface(nullptr);
    return *this;
}

const std::unique_ptr<general::Header>& Client::header() {
    return impl_->header();
}

Client& Client::header(const general::Header& header) {
    impl_->header(header);
    return *this;
}

Client& Client::header(general::Header&& header) {
    impl_->header(std::move(header));
    return *this;
}

Client& Client::header(std::nullptr_t) {
    impl_->header(nullptr);
    return *this;
}

const std::unique_ptr<general::Cookie>& Client::cookie() {
    return impl_->cookie();
}

Client& Client::cookie(const general::Cookie& cookie) {
    impl_->cookie(cookie);
    return *this;
}

Client& Client::cookie(general::Cookie&& cookie) {
    impl_->cookie(std::move(cookie));
    return *this;
}

Client& Client::cookie(std::nullptr_t) {
    impl_->cookie(nullptr);
    return *this;
}

const std::unique_ptr<Client::ErrorCallback>& Client::on_error() {
    return impl_->on_error();
}

Client& Client::on_error(const ErrorCallback& callback) {
    impl_->on_error(callback);
    return *this;
}

Client& Client::on_error(ErrorCallback&& callback) {
    impl_->on_error(std::move(callback));
    return *this;
}

Client& Client::on_error(std::nullptr_t) {
    impl_->on_error(nullptr);
    return *this;
}

const std::unique_ptr<Client::EstablishCallback>& Client::on_establish() {
    return impl_->on_establish();
}

Client& Client::on_establish(const EstablishCallback& callback) {
    impl_->on_establish(callback);
    return *this;
}

Client& Client::on_establish(EstablishCallback&& callback) {
    impl_->on_establish(std::move(callback));
    return *this;
}

Client& Client::on_establish(std::nullptr_t) {
    impl_->on_establish(nullptr);
    return *this;
}

const std::unique_ptr<Client::ReceiveCallback>& Client::on_receive() {
    return impl_->on_receive();
}

Client& Client::on_receive(const ReceiveCallback& callback) {
    impl_->on_receive(callback);
    return *this;
}

Client& Client::on_receive(ReceiveCallback&& callback) {
    impl_->on_receive(std::move(callback));
    return *this;
}

Client& Client::on_receive(std::nullptr_t) {
    impl_->on_receive(nullptr);
    return *this;
}

const std::unique_ptr<Client::CloseCallback>& Client::on_close() {
    return impl_->on_close();
}

Client& Client::on_close(const CloseCallback& callback) {
    impl_->on_close(callback);
    return *this;
}

Client& Client::on_close(CloseCallback&& callback) {
    impl_->on_close(std::move(callback));
    return *this;
}

Client& Client::on_close(std::nullptr_t) {
    impl_->on_close(nullptr);
    return *this;
}

Client& Client::send(const Message& message) {
    impl_->send(message);
    return *this;
}

Client& Client::send(Message&& message) {
    impl_->send(std::move(message));
    return *this;
}

} // namespace websocket
} // namespace network
} // namespace utils
} // namespace rayalto
