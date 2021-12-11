#include "exceptions.hpp"

#include <string>
#include <stdexcept>
#include <cerrno>
#include <cstring>

namespace rayalto {
namespace utils {
namespace exceptions {

Exception::Exception(const std::string& type,
                     const std::string& where,
                     const std::string& message) {
    set_message(type, where, message);
}

Exception::~Exception() noexcept {}
const char* Exception::what() const noexcept {
    return message_.c_str();
}

void Exception::set_message(const std::string& type,
                            const std::string& where,
                            const std::string& message) {
    if (!message_.empty()) {
        message_.clear();
    }

    message_.reserve(23 // Space for format characters
                     + where.size() + type.size() + message.size());
    message_.append("[");
    message_.append(where);
    message_.append("] -> ");
    message_.append(type);
    message_.append(":\n               ");
    message_.append(message);
}

SyscallError::SyscallError(const std::string& where) {
    set_message("SyscallError(error " + std::to_string(errno) + ")",
                where,
                std::strerror(errno));
}

} // namespace exceptions
} // namespace utils
} // namespace rayalto
