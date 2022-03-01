#include "request/authentication.h"

#include <string>
#include <utility>

namespace rayalto {
namespace utils {
namespace request {

Authentication::Authentication(const std::string& username,
                               const std::string& password) :
    username_(username), password_(password) {}

Authentication::Authentication(const Authentication& authentication) :
    username_(authentication.username_), password_(authentication.password_) {}

Authentication::Authentication(Authentication&& authentication) noexcept :
    username_(std::move(authentication.username_)),
    password_(std::move(authentication.password_)) {}

Authentication& Authentication::operator=(
    const Authentication& authentication) {
    username_ = authentication.username_;
    password_ = authentication.password_;
    return *this;
}

Authentication& Authentication::operator=(
    Authentication&& authentication) noexcept {
    username_ = std::move(authentication.username_);
    password_ = std::move(authentication.password_);
    return *this;
}

Authentication::~Authentication() {}

void Authentication::username(const std::string& username) {
    username_ = username;
}

void Authentication::username(std::string&& username) {
    username_ = std::move(username);
}

std::string Authentication::username() {
    return username_;
}

void Authentication::password(const std::string& password) {
    password_ = password;
}

void Authentication::password(std::string&& password) {
    password_ = std::move(password);
}

std::string Authentication::password() {
    return password_;
}

bool Authentication::empty() {
    return (username_.empty() || password_.empty());
}

void Authentication::clear() {
    if (!username_.empty()) {
        username_.clear();
    }
    if (!password_.empty()) {
        password_.clear();
    }
}

const char* Authentication::c_str() {
    if (empty()) {
        return nullptr;
    }
    auth_str_ = username_ + ':' + password_;
    return auth_str_.c_str();
}

} // namespace request
} // namespace utils
} // namespace rayalto
