#include "rautils/network/request.h"

#include <utility>

namespace rayalto {
namespace utils {
namespace network {

bool& Request::MimePart::is_file() {
    return is_file_;
}

const bool& Request::MimePart::is_file() const {
    return is_file_;
}

Request::MimePart& Request::MimePart::is_file(const bool& file) {
    is_file_ = file;
    return *this;
}

Request::MimePart& Request::MimePart::is_file(bool&& file) {
    is_file_ = std::move(file);
    return *this;
}

std::string& Request::MimePart::data() {
    return data_;
}

const std::string& Request::MimePart::data() const {
    return data_;
}

Request::MimePart& Request::MimePart::data(const std::string& data) {
    data_ = data;
    return *this;
}

Request::MimePart& Request::MimePart::data(std::string&& data) {
    data_ = std::move(data);
    return *this;
}

Request::MimePart& Request::MimePart::data(char* data, std::size_t length) {
    data_ = std::string(data, length);
    return *this;
}

std::string& Request::MimePart::type() {
    return type_;
}

const std::string& Request::MimePart::type() const {
    return type_;
}

Request::MimePart& Request::MimePart::type(const std::string& type) {
    type_ = type;
    return *this;
}

Request::MimePart& Request::MimePart::type(std::string&& type) {
    type_ = std::move(type);
    return *this;
}

std::string& Request::MimePart::file_name() {
    return file_name_;
}

const std::string& Request::MimePart::file_name() const {
    return file_name_;
}

Request::MimePart& Request::MimePart::file_name(const std::string& file_name) {
    file_name_ = file_name;
    return *this;
}

Request::MimePart& Request::MimePart::file_name(std::string&& file_name) {
    file_name_ = std::move(file_name);
    return *this;
}

} // namespace network
} // namespace utils
} // namespace rayalto
