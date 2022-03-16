#ifndef RA_UTILS_REQUEST_MIME_PARTS_H_
#define RA_UTILS_REQUEST_MIME_PARTS_H_

#include <initializer_list>
#include <utility>

#include <curl/curl.h>

#include "request/mime_part.h"
#include "util/map_handler.h"

namespace rayalto {
namespace utils {
namespace request {

class MimeParts : public util::Map<MimePart> {
public:
    MimeParts(
        std::initializer_list<std::pair<const std::string, MimePart>> pairs);
    MimeParts(const util::Map<MimePart>& map);
    MimeParts(util::Map<MimePart>&& map);
    MimeParts() = default;
    MimeParts(const MimeParts& mime_parts) = default;
    MimeParts(MimeParts&& mime_parts) noexcept = default;
    MimeParts& operator=(const MimeParts& mime_parts) = default;
    MimeParts& operator=(MimeParts&& mime_parts) noexcept = default;

    virtual ~MimeParts() override = default;
};

} // namespace request
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_REQUEST_MIME_PARTS_H_
