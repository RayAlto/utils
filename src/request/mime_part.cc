#include "request/mime_part.h"

#include <utility>

namespace rayalto {
namespace utils {
namespace request {

bool& MimePart::is_file() {
    return is_file_;
}

const bool& MimePart::is_file() const {
    return is_file_;
}

MimePart& MimePart::is_file(const bool& file) {
    is_file_ = file;
    return *this;
}

MimePart& MimePart::is_file(bool&& file) {
    is_file_ = std::move(file);
    return *this;
}

std::string& MimePart::data() {
    return data_;
}

const std::string& MimePart::data() const {
    return data_;
}

MimePart& MimePart::data(const std::string& data) {
    data_ = data;
    return *this;
}

MimePart& MimePart::data(std::string&& data) {
    data_ = std::move(data);
    return *this;
}

MimePart& MimePart::data(char* data, std::size_t length) {
    data_ = std::string(data, length);
    return *this;
}

std::string& MimePart::type() {
    return type_;
}

const std::string& MimePart::type() const {
    return type_;
}

MimePart& MimePart::type(const std::string& type) {
    type_ = type;
    return *this;
}

MimePart& MimePart::type(std::string&& type) {
    type_ = std::move(type);
    return *this;
}

std::string& MimePart::file_name() {
    return file_name_;
}

const std::string& MimePart::file_name() const {
    return file_name_;
}

MimePart& MimePart::file_name(const std::string& file_name) {
    file_name_ = file_name;
    return *this;
}

MimePart& MimePart::file_name(std::string&& file_name) {
    file_name_ = std::move(file_name);
    return *this;
}

} // namespace request
} // namespace utils
} // namespace rayalto
