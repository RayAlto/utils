#ifndef RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_MESSAGE_H_
#define RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_MESSAGE_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace rayalto::utils::network::websocket {

class Message {
public:
    static const std::size_t MAX_LENGTH;
    enum class Type : std::uint8_t { TEXT, BINARY };

    Message() = default;
    Message(const Message&) = default;
    Message(Message&&) noexcept = default;
    Message& operator=(const Message&) = default;
    Message& operator=(Message&&) noexcept = default;

    virtual ~Message() = default;

    explicit Message(const std::string& text);
    explicit Message(std::string&& text);

    explicit Message(const std::vector<unsigned char>& binary);
    explicit Message(std::vector<unsigned char>&& binary);

    [[nodiscard]] const Type& type() const;
    Type& type();

    Message& type(const Type& type);

    [[nodiscard]] const std::string& text() const;
    std::string& text();

    Message& text(const std::string& text);
    Message& text(std::string&& text);

    [[nodiscard]] const std::vector<unsigned char>& binary() const;
    std::vector<unsigned char>& binary();

    Message& binary(const std::vector<unsigned char>& binary);
    Message& binary(std::vector<unsigned char>&& binary);

    [[nodiscard]] bool empty() const;

    [[nodiscard]] std::size_t length() const;

    void reserve(std::size_t new_capacity);

    unsigned char* data();

protected:
    std::variant<std::string, std::vector<unsigned char>> data_;
    Type type_ = Type::TEXT;
};

} // namespace rayalto::utils::network::websocket

#endif // RA_UTILS_RAUTILS_NETWORK_WEBSOCKET_MESSAGE_H_
