#include "rautils/network/general/url.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>

#include "rautils/network/general/authentication.h"
#include "rautils/network/general/query.h"
#include "rautils/string/strtool.h"

namespace rayalto {
namespace utils {
namespace network {
namespace general {

Url::Url(const std::string& url) {
    from_string_(url);
}

Url::Url(const std::string& scheme,
         const Authentication& userinfo,
         const std::string& host,
         const std::uint16_t& port,
         const std::string& path,
         const Query& query,
         const std::string& fragment) :
    scheme_(std::make_unique<std::string>(scheme)),
    userinfo_(std::make_unique<Authentication>(userinfo)),
    host_(std::make_unique<std::string>(host)),
    port_(std::make_unique<std::uint16_t>(port)),
    path_(std::make_unique<std::string>(path)),
    query_(std::make_unique<Query>(query)),
    fragment_(std::make_unique<std::string>(fragment)) {}

Url::Url(std::string&& scheme,
         Authentication&& userinfo,
         std::string&& host,
         std::uint16_t&& port,
         std::string&& path,
         Query&& query,
         std::string&& fragment) :
    scheme_(std::make_unique<std::string>(std::move(scheme))),
    userinfo_(std::make_unique<Authentication>(std::move(userinfo))),
    host_(std::make_unique<std::string>(std::move(host))),
    port_(std::make_unique<std::uint16_t>(std::move(port))),
    path_(std::make_unique<std::string>(std::move(path))),
    query_(std::make_unique<Query>(std::move(query))),
    fragment_(std::make_unique<std::string>(std::move(fragment))) {}

Url::Url(const Url& url) :
    scheme_(url.scheme_ == nullptr
                ? nullptr
                : std::make_unique<std::string>(*url.scheme_)),
    userinfo_(url.userinfo_ == nullptr
                  ? nullptr
                  : std::make_unique<Authentication>(*url.userinfo_)),
    host_(url.host_ == nullptr ? nullptr
                               : std::make_unique<std::string>(*url.host_)),
    port_(url.port_ == nullptr ? nullptr
                               : std::make_unique<std::uint16_t>(*url.port_)),
    path_(url.path_ == nullptr ? nullptr
                               : std::make_unique<std::string>(*url.path_)),
    query_(url.query_ == nullptr ? nullptr
                                 : std::make_unique<Query>(*url.query_)),
    fragment_(url.fragment_ == nullptr
                  ? nullptr
                  : std::make_unique<std::string>(*url.fragment_)),
    url_str_(url.url_str_ == nullptr
                 ? nullptr
                 : std::make_unique<std::string>(*url.url_str_)) {}

Url& Url::operator=(const Url& url) {
    scheme_ = url.scheme_ == nullptr
                  ? nullptr
                  : std::make_unique<std::string>(*url.scheme_);
    userinfo_ = url.userinfo_ == nullptr
                    ? nullptr
                    : std::make_unique<Authentication>(*url.userinfo_);
    host_ = url.host_ == nullptr ? nullptr
                                 : std::make_unique<std::string>(*url.host_);
    port_ = url.port_ == nullptr ? nullptr
                                 : std::make_unique<std::uint16_t>(*url.port_);
    path_ = url.path_ == nullptr ? nullptr
                                 : std::make_unique<std::string>(*url.path_);
    query_ =
        url.query_ == nullptr ? nullptr : std::make_unique<Query>(*url.query_);
    fragment_ = url.fragment_ == nullptr
                    ? nullptr
                    : std::make_unique<std::string>(*url.fragment_);
    url_str_ = url.url_str_ == nullptr
                   ? nullptr
                   : std::make_unique<std::string>(*url.url_str_);
    return *this;
}

const std::unique_ptr<std::string>& Url::scheme() {
    return scheme_;
}

Url& Url::scheme(const std::string& scheme) {
    scheme_ = std::make_unique<std::string>(scheme);
    return *this;
}

Url& Url::scheme(std::string&& scheme) {
    scheme_ = std::make_unique<std::string>(std::move(scheme));
    return *this;
}

Url& Url::scheme(std::nullptr_t) {
    scheme_ = nullptr;
    return *this;
}

const std::unique_ptr<Authentication>& Url::userinfo() {
    return userinfo_;
}

Url& Url::userinfo(const Authentication& userinfo) {
    userinfo_ = std::make_unique<Authentication>(userinfo);
    return *this;
}

Url& Url::userinfo(Authentication&& userinfo) {
    userinfo_ = std::make_unique<Authentication>(std::move(userinfo));
    return *this;
}

Url& Url::userinfo(std::nullptr_t) {
    userinfo_ = nullptr;
    return *this;
}

const std::unique_ptr<std::string>& Url::host() {
    return host_;
}

Url& Url::host(const std::string& host) {
    host_ = std::make_unique<std::string>(host);
    return *this;
}

Url& Url::host(std::string&& host) {
    host_ = std::make_unique<std::string>(std::move(host));
    return *this;
}

Url& Url::host(std::nullptr_t) {
    host_ = nullptr;
    return *this;
}

const std::unique_ptr<std::uint16_t>& Url::port() {
    return port_;
}

Url& Url::port(const std::uint16_t& port) {
    port_ = std::make_unique<std::uint16_t>(port);
    return *this;
}

Url& Url::port(std::uint16_t&& port) {
    port_ = std::make_unique<std::uint16_t>(std::move(port));
    return *this;
}

Url& Url::port(std::nullptr_t) {
    port_ = nullptr;
    return *this;
}

const std::unique_ptr<std::string>& Url::path() {
    return path_;
}

Url& Url::path(const std::string& path) {
    path_ = std::make_unique<std::string>(path);
    return *this;
}

Url& Url::path(std::string&& path) {
    path_ = std::make_unique<std::string>(std::move(path));
    return *this;
}

Url& Url::path(std::nullptr_t) {
    path_ = nullptr;
    return *this;
}

const std::unique_ptr<Query>& Url::query() {
    return query_;
}

Url& Url::query(const Query& query) {
    query_ = std::make_unique<Query>(query);
    return *this;
}

Url& Url::query(Query&& query) {
    query_ = std::make_unique<Query>(std::move(query));
    return *this;
}

Url& Url::query(std::nullptr_t) {
    query_ = nullptr;
    return *this;
}

const std::unique_ptr<std::string>& Url::fragment() {
    return fragment_;
}

Url& Url::fragment(const std::string& fragment) {
    fragment_ = std::make_unique<std::string>(fragment);
    return *this;
}

Url& Url::fragment(std::string&& fragment) {
    fragment_ = std::make_unique<std::string>(std::move(fragment));
    return *this;
}

Url& Url::fragment(std::nullptr_t) {
    fragment_ = nullptr;
    return *this;
}

const std::string& Url::to_string() const {
    form_string_();
    return *url_str_;
}

const char* Url::c_str() const {
    form_string_();
    return url_str_->c_str();
}

void Url::from_string_(const std::string& str) {
    std::string::const_iterator str_begin_iter = str.begin();

    std::size_t scheme_end_pos = str.find(':');
    if (scheme_end_pos == std::string::npos) {
        // no scheme component
        /* throw std::invalid_argument("Can not find scheme component in \"" + str */
        /*                             + '"'); */
        return;
    }
    scheme_ = std::make_unique<std::string>(str.substr(0, scheme_end_pos));

    std::size_t path_start_pos = scheme_end_pos + 1;
    std::size_t path_end_pos = path_start_pos;

    if (str[scheme_end_pos + 1] == '/' && str[scheme_end_pos + 2] == '/') {
        // has authority component
        std::size_t userinfo_end_pos = str.find('@', scheme_end_pos + 3);
        std::size_t host_start_pos = 0;
        std::size_t host_end_pos = 0;

        if (userinfo_end_pos == std::string::npos) {
            // no userinfo subcomponent
            host_start_pos = scheme_end_pos + 3;
        }

        else {
            // has userinfo subcomponent
            host_start_pos = userinfo_end_pos + 1;
            std::string::const_iterator userinfo_start_iter =
                str_begin_iter + scheme_end_pos + 3;
            std::string::const_iterator userinfo_end_iter =
                str_begin_iter + userinfo_end_pos;
            std::string::const_iterator username_end_iter =
                std::find(userinfo_start_iter, userinfo_end_iter, ':');
            if (username_end_iter == userinfo_end_iter) {
                // no password, just a user name
                userinfo_ = std::make_unique<Authentication>(
                    std::string(userinfo_start_iter, userinfo_end_iter),
                    std::string {});
            }
            else {
                // user name and password
                userinfo_ = std::make_unique<Authentication>(
                    std::string(userinfo_start_iter, username_end_iter),
                    std::string(username_end_iter + 1, userinfo_end_iter));
            }
        }

        if (str[host_start_pos] == '[') {
            // is IPv6 address
            host_end_pos = str.find(']', host_start_pos);

            if (host_end_pos == std::string::npos) {
                // IPv6 address not complete
                /* throw std::invalid_argument('"' + str.substr(host_start_pos) */
                /*                             + "\": invalid IPv6 address"); */
                return;
            }

            host_end_pos += 1;
            host_ = std::make_unique<std::string>(
                str.substr(host_start_pos, host_end_pos - host_start_pos));
        }

        else {
            // normal address
            host_end_pos = str.find_first_of(":/?#", host_start_pos);

            if (host_end_pos == std::string::npos) {
                // remaining characters form the host component, path is empty
                // and there are no other components
                host_ =
                    std::make_unique<std::string>(str.substr(host_start_pos));
                path_ = std::make_unique<std::string>();
                return;
            }

            host_ = std::make_unique<std::string>(
                str.substr(host_start_pos, host_end_pos - host_start_pos));
        }

        path_start_pos = host_end_pos;

        if (str[host_end_pos] == ':') {
            // port subcomponent
            std::string::const_iterator port_start_iter =
                str_begin_iter + host_end_pos + 1;
            std::string::const_iterator port_end_iter =
                std::find_if_not(port_start_iter,
                                 str.end(),
                                 static_cast<int (*)(int)>(std::isdigit));
            port_ = std::make_unique<std::uint16_t>(
                std::stoi(std::string(port_start_iter, port_end_iter)));

            path_start_pos += port_end_iter - port_start_iter;
        }
    }

    path_end_pos = str.find_first_of("?#", path_start_pos);

    if (path_end_pos == std::string::npos) {
        // remaining characters form the path component
        // and there are no other components
        path_ = std::make_unique<std::string>(str.substr(path_start_pos));
        return;
    }

    path_ = std::make_unique<std::string>(
        str.substr(path_start_pos, path_end_pos - path_start_pos));

    std::size_t query_start_pos = path_end_pos + 1;
    std::size_t query_end_pos = 0;
    if (str[path_end_pos] == '?') {
        // query component
        query_ = std::make_unique<Query>();
        query_end_pos = str.find('#', query_start_pos);
        if (query_end_pos == std::string::npos) {
            // remaining characters form the query component
            // and there are no other components
            for (const std::string& pair_str :
                 string::split(str.substr(query_start_pos), '&')) {
                query_->update(string::split_once(pair_str, '='));
            }
            return;
        }
        else {
            for (const std::string& pair_str :
                 string::split(str.substr(query_start_pos,
                                          query_end_pos - query_start_pos),
                               '&')) {
                query_->update(string::split_once(pair_str, '='));
            }
        }
    }

    std::size_t fragment_start_pos = str.find('#', query_end_pos);
    if (fragment_start_pos != std::string::npos) {
        // remaining characters form the fragment component
        // and there are no other components
        fragment_start_pos += 1;
        fragment_ =
            std::make_unique<std::string>(str.substr(fragment_start_pos));
        return;
    }

    return;
}

void Url::form_string_() const {
    url_str_ = std::make_unique<std::string>();
    std::string& url_str = *url_str_;
    if (scheme_ == nullptr || path_ == nullptr) {
        // necessary components
        return;
    }
    url_str += *scheme_;
    url_str += ':';
    if (userinfo_ != nullptr || host_ != nullptr || port_ != nullptr) {
        url_str += "//";
    }
    if (userinfo_ != nullptr) {
        url_str += userinfo_->username();
        if (!userinfo_->password().empty()) {
            url_str += ':';
            url_str += userinfo_->password();
        }
        url_str += '@';
    }
    if (host_ != nullptr) {
        url_str += *host_;
    }
    if (port_ != nullptr) {
        url_str += ':';
        url_str += std::to_string(*port_);
    }
    url_str += *path_;
    if (query_ != nullptr) {
        url_str += '?';
        url_str += string::kv_format(
            query_->base_container(), '=', false, '&', false, false);
    }
    if (fragment_ != nullptr) {
        url_str += '#';
        url_str += *fragment_;
    }
}

} // namespace general
} // namespace network
} // namespace utils
} // namespace rayalto
