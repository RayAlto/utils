#ifndef RA_UTILS_WEBSOCKET_MESSAGE_TYPE_H_
#define RA_UTILS_WEBSOCKET_MESSAGE_TYPE_H_

#include <cstdint>

namespace rayalto {
namespace utils {
namespace websocket {

enum class MessageType : std::uint8_t { TEXT, BINARY };

} // namespace websocket
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_WEBSOCKET_MESSAGE_TYPE_H_
