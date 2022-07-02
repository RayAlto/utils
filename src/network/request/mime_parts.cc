#include "rautils/network/request.h"

#include <utility>

namespace rayalto::utils::network {

Request::MimeParts::MimeParts(
    std::initializer_list<std::pair<const std::string, MimePart>> pairs) :
    misc::Map<MimePart>(pairs) {}

Request::MimeParts::MimeParts(const misc::Map<MimePart>& map) :
    misc::Map<MimePart>(map) {}

Request::MimeParts::MimeParts(misc::Map<MimePart>&& map) :
    misc::Map<MimePart>(map) {}

} // namespace rayalto::utils::network
