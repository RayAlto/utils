#include "request/mime_parts.h"

#include <utility>

namespace rayalto {
namespace utils {
namespace request {

MimeParts::MimeParts(
    std::initializer_list<std::pair<const std::string, MimePart>> pairs) :
    util::Map<MimePart>(pairs) {}

MimeParts::MimeParts(const util::Map<MimePart>& map) :
    util::Map<MimePart>(map) {}

MimeParts::MimeParts(util::Map<MimePart>&& map) : util::Map<MimePart>(map) {}

} // namespace request
} // namespace utils
} // namespace rayalto
