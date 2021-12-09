#ifndef RA_UTILS_INCLUDE_EXCEPTIONS_HPP_
#define RA_UTILS_INCLUDE_EXCEPTIONS_HPP_

#include <string>

namespace rayalto {
namespace utils {

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

    virtual ~Exception() noexcept;
    virtual const char* what() const noexcept;

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

} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_INCLUDE_EXCEPTIONS_HPP_
