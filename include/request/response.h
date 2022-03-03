#ifndef RA_UTILS_REQUEST_RESPONSE_HPP_
#define RA_UTILS_REQUEST_RESPONSE_HPP_

#include <string>
#include <cstdint>

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
    double pre_transfer;
    // time elapsed from start until the first byte is received
    double start_transfer;
};

struct Speed {
    std::int64_t download;
    std::int64_t upload;
};

struct ByteTransfered {
    std::int64_t download;
    std::int64_t upload;
};

struct LocalInfo {
    std::string ip;
    long port;
};

} // namespace response

struct Response {
    std::string message;
    std::string body;
    long code;
    long http_version;
    Header header;
    Cookie cookie;
    response::TimeElapsed time_elapsed;
    response::ByteTransfered byte_transfered;
    response::Speed speed;
    response::LocalInfo local_info;
    std::string verbose;
};

} // namespace request
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_RESPONSE_HPP_
