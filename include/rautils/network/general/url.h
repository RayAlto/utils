#ifndef RA_UTILS_RAUTILS_NETWORK_GENERAL_URL_H_
#define RA_UTILS_RAUTILS_NETWORK_GENERAL_URL_H_

#include <cstdint>
#include <memory>
#include <string>

#include "rautils/misc/status.h"
#include "rautils/network/general/authentication.h"
#include "rautils/network/general/query.h"

namespace rayalto {
namespace utils {
namespace network {
namespace general {

/**
 * A simple lazy url parser which is so lazy to do much validation
 * https://en.wikipedia.org/wiki/URL
 */
class Url {
public:
    Url(const std::string& url);
    Url(const std::string& scheme,
        const Authentication& userinfo,
        const std::string& host,
        const std::uint16_t& port,
        const std::string& path,
        const Query& query,
        const std::string& fragment);
    Url(std::string&& scheme,
        Authentication&& userinfo,
        std::string&& host,
        std::uint16_t&& port,
        std::string&& path,
        Query&& query,
        std::string&& fragment);
    Url() = default;
    Url(const Url& url);
    Url(Url&&) noexcept = default;
    Url& operator=(const Url& url);
    Url& operator=(Url&&) noexcept = default;

    virtual ~Url() = default;

    const std::unique_ptr<std::string>& scheme();
    Url& scheme(const std::string& scheme);
    Url& scheme(std::string&& scheme);
    Url& scheme(std::nullptr_t);

    const std::unique_ptr<Authentication>& userinfo();
    Url& userinfo(const Authentication& userinfo);
    Url& userinfo(Authentication&& userinfo);
    Url& userinfo(std::nullptr_t);

    const std::unique_ptr<std::string>& host();
    Url& host(const std::string& host);
    Url& host(std::string&& host);
    Url& host(std::nullptr_t);

    const std::unique_ptr<std::uint16_t>& port();
    Url& port(const std::uint16_t& port);
    Url& port(std::uint16_t&& port);
    Url& port(std::nullptr_t);

    const std::unique_ptr<std::string>& path();
    Url& path(const std::string& path);
    Url& path(std::string&& path);
    Url& path(std::nullptr_t);

    const std::unique_ptr<Query>& query();
    Url& query(const Query& query);
    Url& query(Query&& query);
    Url& query(std::nullptr_t);

    const std::unique_ptr<std::string>& fragment();
    Url& fragment(const std::string& fragment);
    Url& fragment(std::string&& fragment);
    Url& fragment(std::nullptr_t);

    const std::string& to_string() const;
    const char* c_str() const;

protected:
    std::unique_ptr<std::string> scheme_ = nullptr;
    std::unique_ptr<Authentication> userinfo_ = nullptr;
    std::unique_ptr<std::string> host_ = nullptr;
    std::unique_ptr<std::uint16_t> port_ = nullptr;
    std::unique_ptr<std::string> path_ = nullptr;
    std::unique_ptr<Query> query_ = nullptr;
    std::unique_ptr<std::string> fragment_ = nullptr;
    mutable std::unique_ptr<std::string> url_str_ = nullptr;

protected:
    void from_string_(const std::string& str);
    void form_string_() const;
};

} // namespace general
} // namespace network
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_NETWORK_GENERAL_URL_H_
