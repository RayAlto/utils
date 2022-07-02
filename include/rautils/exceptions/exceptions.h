#ifndef RA_UTILS_EXCEPTIONS_EXCEPTIONS_HPP_
#define RA_UTILS_EXCEPTIONS_EXCEPTIONS_HPP_

#include <exception>
#include <string>

namespace rayalto::utils::exceptions {

/**
 * Base class for exceptions
 * Example:
 *      throw Exception("Type", "Where", "Message");
 * 
 * Output (shell):
 *      terminate called after throwing an instance of 'rayalto::utils::Exception'
 *        what():  [where] -> type:
 *                     message
 *      [1]    12345 abort (core dumped)  /home/user/programs/cpp/foo/build/foo
 */
class Exception : public std::exception {
public:
    Exception(const std::string& type,
              const std::string& where,
              const std::string& message);
    Exception() = default;
    Exception(const Exception&) = delete;
    Exception(Exception&&) noexcept = default;
    Exception& operator=(const Exception&) = delete;
    Exception& operator=(Exception&&) noexcept = default;

    ~Exception() noexcept override = default;
    [[nodiscard]] const char* what() const noexcept override;

protected:
    void set_message(const std::string& type,
                     const std::string& where,
                     const std::string& message);

    std::string message_;
};

/**
 * Automatically get error name from errno set by system calls
 * Example:
 *      if(fork() == -1) {
 *          throw SyscallError("Here");
 *      }
 */
class SyscallError : public Exception {
public:
    explicit SyscallError(const std::string& where);
};

/**
 * Automatically get error info from OpenSSL
 * Example:
 *      if(EVP_DecodeBlock(t, f, n) == -1) {
 *          throw OpensslError("Here");
 *      }
 */
class OpensslError : public Exception {
public:
    explicit OpensslError(const std::string& where);
};

} // namespace rayalto::utils::exceptions

#endif // RA_UTILS_EXCEPTIONS_EXCEPTIONS_HPP_
