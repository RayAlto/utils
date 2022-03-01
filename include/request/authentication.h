#ifndef RA_UTILS_REQUEST_AUTHENTICATION_HPP_
#define RA_UTILS_REQUEST_AUTHENTICATION_HPP_

#include <string>

namespace rayalto {
namespace utils {
namespace request {

class Authentication {
public:
    Authentication() = default;
    Authentication(const std::string& username, const std::string& password);
    Authentication(const Authentication& authentication);
    Authentication(Authentication&& authentication) noexcept;
    Authentication& operator=(const Authentication& authentication);
    Authentication& operator=(Authentication&& authentication) noexcept;

    virtual ~Authentication();

    void username(const std::string& username);
    void username(std::string&& username);
    std::string username();
    void password(const std::string& password);
    void password(std::string&& password);
    std::string password();

    // if username or password is empty
    bool empty();
    // reset
    void clear();

    virtual const char* c_str();

protected:
    std::string username_;
    std::string password_;
    std::string auth_str_;
};

} // namespace request
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_AUTHENTICATION_HPP_
