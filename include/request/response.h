#ifndef RA_UTILS_REQUEST_RESPONSE_HPP_
#define RA_UTILS_REQUEST_RESPONSE_HPP_

#include <string>

#include "cookie.h"
#include "header.h"

namespace rayalto {
namespace utils {
namespace request {

namespace response {

struct TimeElapsed {
    // total time elapsed
    double all;
    // time elapsed from start until name resolving completed
    double name_resolve;
    // time elapsed from start until connection(remote_host/proxy) completed
    double connect;
    // time elapsed from start until SSL/SSH handshake completed
    double handshake;
    // time elapsed until file transfer start
    double pretransfer;
};

} // namespace response

struct Response {
    std::string message;
    std::string body;
    long code;
    long http_version;
    response::TimeElapsed time_elapsed;
    Header header;
    Cookie cookie;
};

} // namespace request
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_RESPONSE_HPP_
