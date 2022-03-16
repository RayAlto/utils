#ifndef RA_UTILS_REQUEST_MIME_PART_H_
#define RA_UTILS_REQUEST_MIME_PART_H_

#include <cstddef>
#include <string>

#include <curl/curl.h>
namespace rayalto {
namespace utils {
namespace request {

class MimePart {
public:
    MimePart() = default;
    MimePart(const MimePart&) = default;
    MimePart(MimePart&&) noexcept = default;
    MimePart& operator=(const MimePart&) = default;
    MimePart& operator=(MimePart&&) noexcept = default;

    virtual ~MimePart() = default;

    // is a file?
    bool& is_file();
    const bool& is_file() const;
    MimePart& is_file(const bool& file);
    MimePart& is_file(bool&& file);

    // part data or a local file name
    std::string& data();
    const std::string& data() const;
    MimePart& data(const std::string& data);
    MimePart& data(std::string&& data);
    MimePart& data(char* data, std::size_t length);

    // mime type
    std::string& type();
    const std::string& type() const;
    MimePart& type(const std::string& type);
    MimePart& type(std::string&& type);

    // [optional] remote file name, take effect when file() is true
    std::string& file_name();
    const std::string& file_name() const;
    MimePart& file_name(const std::string& file_name);
    MimePart& file_name(std::string&& file_name);

protected:
    // is a file?
    bool is_file_ = false;
    // part data or file name
    std::string data_;
    // mime type
    std::string type_;
    // remote file name
    std::string file_name_;
};

} // namespace request
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_MIME_PART_H_
