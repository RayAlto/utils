#include "rautils/network/websocket/message.h"

#include <cstddef>
#include <limits>
#include <string>
#include <utility>
#include <variant>

#include "libwebsockets.h"

namespace rayalto::utils::network::websocket {

const std::size_t Message::MAX_LENGTH =
    std::numeric_limits<std::size_t>::max() - LWS_PRE;

Message::Message(const std::string& text) : data_(text) {}

Message::Message(std::string&& text) : data_(std::move(text)) {}

Message::Message(const std::vector<unsigned char>& binary) :
    data_(binary), type_(Type::BINARY) {}

Message::Message(std::vector<unsigned char>&& binary) :
    data_(std::move(binary)), type_(Type::BINARY) {}

const Message::Type& Message::type() const {
    return type_;
}

Message::Type& Message::type() {
    return type_;
}

Message& Message::type(const Type& type) {
    type_ = type;
    return *this;
}

const std::string& Message::text() const {
    return std::get<std::string>(data_);
}

std::string& Message::text() {
    return std::get<std::string>(data_);
}

Message& Message::text(const std::string& text) {
    type_ = Type::TEXT;
    data_ = text;
    return *this;
}

Message& Message::text(std::string&& text) {
    type_ = Type::TEXT;
    data_ = std::move(text);
    return *this;
}

const std::vector<unsigned char>& Message::binary() const {
    return std::get<std::vector<unsigned char>>(data_);
}

std::vector<unsigned char>& Message::binary() {
    return std::get<std::vector<unsigned char>>(data_);
}

Message& Message::binary(const std::vector<unsigned char>& binary) {
    type_ = Type::BINARY;
    data_ = binary;
    return *this;
}

Message& Message::binary(std::vector<unsigned char>&& binary) {
    type_ = Type::BINARY;
    data_ = std::move(binary);
    return *this;
}

bool Message::empty() const {
    switch (type_) {
    case Type::TEXT: return std::get<std::string>(data_).empty(); break;
    case Type::BINARY:
        return std::get<std::vector<unsigned char>>(data_).empty();
        break;
    default: return true; break;
    }
}

std::size_t Message::length() const {
    switch (type_) {
    case Type::TEXT: return std::get<std::string>(data_).length(); break;
    case Type::BINARY:
        return std::get<std::vector<unsigned char>>(data_).size();
        break;
    default: return 0; break;
    }
}

void Message::reserve(std::size_t new_capacity) {
    switch (type_) {
    case Type::TEXT: std::get<std::string>(data_).reserve(new_capacity); break;
    case Type::BINARY:
        std::get<std::vector<unsigned char>>(data_).reserve(new_capacity);
        break;
    default: return; break;
    }
}

unsigned char* Message::data() {
    switch (type_) {
    case Type::TEXT:
        return reinterpret_cast<unsigned char*>(
            std::get<std::string>(data_).data());
        break;
    case Type::BINARY:
        return std::get<std::vector<unsigned char>>(data_).data();
        break;
    default: return nullptr; break;
    }
}

} // namespace rayalto::utils::network::websocket
