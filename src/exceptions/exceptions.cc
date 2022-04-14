#include "exceptions/exceptions.h"
#include <openssl/err.h>

#include <cerrno> // errno(marco)
#include <cstring> // std::strerror
#include <string>

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

    message_.reserve(10 // space for format characters
                     + where.size() + type.size() + message.size());
    message_.append("[");
    message_.append(where);
    message_.append("] -> ");
    message_.append(type);
    message_.append(":\n\t\t");
    message_.append(message);
}

SyscallError::SyscallError(const std::string& where) {
    set_message("SyscallError(error " + std::to_string(errno) + ")",
                where,
                std::strerror(errno));
}

OpensslError::OpensslError(const std::string& where) {
    unsigned long error_code = ERR_get_error();
    char error_buffer[256];
    ERR_error_string(error_code, error_buffer);
    set_message("OpenSSL(" + std::to_string(error_code) + ")",
                where,
                std::string(error_buffer));
}

} // namespace exceptions
} // namespace utils
} // namespace rayalto
