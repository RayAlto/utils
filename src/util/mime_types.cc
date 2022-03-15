#include "util/mime_types.h"

#include <cstddef>
#include <string>

namespace rayalto {
namespace utils {
namespace util {

namespace {

std::string extension_of(const std::string& filename) {
    std::size_t last_dot = filename.find_last_of('.');
    if (last_dot == std::string::npos) {
        return filename;
    }
    return filename.substr(last_dot + 1);
}

} // anonymous namespace

bool MimeTypes::know(const std::string& filename) {
    return ext2mime_.find(extension_of(filename)) != ext2mime_.end();
}

std::string MimeTypes::get(const std::string& filename,
                           const std::string& fallback) {
    std::map<std::string, std::string>::const_iterator found =
        ext2mime_.find(extension_of(filename));
    return found == ext2mime_.end() ? fallback : found->second;
}

const std::string MimeTypes::default_text {"text/plain"};
const std::string MimeTypes::default_binary {"application/octet-stream"};

} // namespace util
} // namespace utils
} // namespace rayalto
