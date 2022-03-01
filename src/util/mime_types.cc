#include "util/mime_types.h"

#include <string>

namespace rayalto {
namespace utils {
namespace util {

bool MimeTypes::know(const std::string& extension) {
    return ext2mime_.find(extension) != ext2mime_.end();
}

std::string MimeTypes::get(const std::string& extension,
                           const std::string& fallback) {
    std::map<std::string, std::string>::const_iterator found =
        ext2mime_.find(extension);
    return found == ext2mime_.end() ? fallback : found->second;
}

const std::string MimeTypes::default_text {"text/plain"};
const std::string MimeTypes::default_binary {"application/octet-stream"};

} // namespace util
} // namespace utils
} // namespace rayalto
