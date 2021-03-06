#ifndef RA_UTILS_UTIL_MIME_TYPES_H_
#define RA_UTILS_UTIL_MIME_TYPES_H_

#include <map>
#include <string>

namespace rayalto::utils::misc {

class MimeTypes {
public:
    MimeTypes() = default;
    MimeTypes(const MimeTypes&) = delete;
    MimeTypes(MimeTypes&&) noexcept = delete;
    MimeTypes& operator=(const MimeTypes&) = delete;
    MimeTypes& operator=(MimeTypes&&) noexcept = delete;

    virtual ~MimeTypes() = default;

    // check if I know this file extension
    static bool know(const std::string& filename);

    // get mime type of the file extension
    // <fallback> if I do not know about this extension
    static std::string get(const std::string& filename,
                           const std::string& fallback = default_binary);

    static const std::string default_text;
    static const std::string default_binary;

protected:
    // file extension => mime type
    static const std::map<std::string, std::string> ext2mime_;
};

} // namespace rayalto::utils::misc

#endif // RA_UTILS_UTIL_MIME_TYPES_H_
