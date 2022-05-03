#include "rautils/network/request.h"

#include <string>
#include <utility>

namespace rayalto {
namespace utils {
namespace network {

Request::Authentication::Authentication(const std::string& username,
                                        const std::string& password) :
    username_(username), password_(password) {}

Request::Authentication::Authentication(const Authentication& authentication) :
    username_(authentication.username_), password_(authentication.password_) {}

Request::Authentication::Authentication(
    Authentication&& authentication) noexcept :
    username_(std::move(authentication.username_)),
    password_(std::move(authentication.password_)) {}

Request::Authentication& Request::Authentication::operator=(
    const Authentication& authentication) {
    username_ = authentication.username_;
    password_ = authentication.password_;
    return *this;
}

Request::Authentication& Request::Authentication::operator=(
    Authentication&& authentication) noexcept {
    username_ = std::move(authentication.username_);
    password_ = std::move(authentication.password_);
    return *this;
}

Request::Authentication::~Authentication() {}

void Request::Authentication::username(const std::string& username) {
    username_ = username;
}

void Request::Authentication::username(std::string&& username) {
    username_ = std::move(username);
}

std::string Request::Authentication::username() {
    return username_;
}

void Request::Authentication::password(const std::string& password) {
    password_ = password;
}

void Request::Authentication::password(std::string&& password) {
    password_ = std::move(password);
}

std::string Request::Authentication::password() {
    return password_;
}

bool Request::Authentication::empty() {
    return (username_.empty() || password_.empty());
}

void Request::Authentication::clear() {
    if (!username_.empty()) {
        username_.clear();
    }
    if (!password_.empty()) {
        password_.clear();
    }
}

const char* Request::Authentication::c_str() {
    if (empty()) {
        return nullptr;
    }
    auth_str_ = username_ + ':' + password_;
    return auth_str_.c_str();
}

} // namespace network
} // namespace utils
} // namespace rayalto
