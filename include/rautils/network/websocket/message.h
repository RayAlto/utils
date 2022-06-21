#ifndef RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_MESSAGE_H_
#define RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_MESSAGE_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace rayalto {
namespace utils {
namespace network {
namespace websocket {

class Message {
public:
    static const std::size_t MAX_LENGTH;
    enum class Type : std::uint8_t { TEXT, BINARY };

public:
    Message() = default;
    Message(const Message&) = default;
    Message(Message&&) noexcept = default;
    Message& operator=(const Message&) = default;
    Message& operator=(Message&&) noexcept = default;

    virtual ~Message() = default;

    Message(const std::string& text);
    Message(std::string&& text);

    Message(const std::vector<unsigned char>& binary);
    Message(std::vector<unsigned char>&& binary);

    const Type& type() const;
    Type& type();

    Message& type(const Type& type);
    Message& type(Type&& type);

    const std::string& text() const;
    std::string& text();

    Message& text(const std::string& text);
    Message& text(std::string&& text);

    const std::vector<unsigned char>& binary() const;
    std::vector<unsigned char>& binary();

    Message& binary(const std::vector<unsigned char>& binary);
    Message& binary(std::vector<unsigned char>&& binary);

    bool empty() const;

    std::size_t length() const;

    void reserve(std::size_t new_capacity);

    unsigned char* data();

protected:
    std::variant<std::string, std::vector<unsigned char>> data_;
    Type type_;
};

} // namespace websocket
} // namespace network
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_MESSAGE_H_
