#ifndef RA_UTILS_RAUTILS_NETWORK_GENERAL_H_
#define RA_UTILS_RAUTILS_NETWORK_GENERAL_H_

#include <initializer_list>
#include <string>
#include <utility>

#include "rautils/misc/map_handler.h"

namespace rayalto {
namespace utils {
namespace network {
namespace general {

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

class Cookie : public misc::DictHandler {
public:
    Cookie(
        std::initializer_list<std::pair<const std::string, std::string>> pairs);
    Cookie(const misc::Dict& map);
    Cookie(misc::Dict&& map);
    Cookie() = default;
    Cookie(const Cookie&);
    Cookie(Cookie&&) noexcept;
    Cookie& operator=(const Cookie&);
    Cookie& operator=(Cookie&&) noexcept;

    virtual ~Cookie() = default;

    // format as "key1=value1; key2=value2; ..."
    const char* c_str();

protected:
    std::string str_;
};

class Header : public misc::DictIcHandler {
public:
    Header(
        std::initializer_list<std::pair<const std::string, std::string>> pairs);
    Header(const misc::DictIC& map);
    Header(misc::DictIC&& map);
    Header() = default;
    Header(const Header& header);
    Header(Header&& header) noexcept;
    Header& operator=(const Header& header);
    Header& operator=(Header&& header) noexcept;

    virtual ~Header() = default;
};

} // namespace general
} // namespace network
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_NETWORK_GENERAL_H_
