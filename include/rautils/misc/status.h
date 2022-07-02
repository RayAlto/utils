#ifndef RA_UTILS_RAUTILS_MISC_STATUS_H_
#define RA_UTILS_RAUTILS_MISC_STATUS_H_

#include <memory>
#include <string>

namespace rayalto::utils::misc {

struct Status {
    bool error = false;
    std::unique_ptr<std::string> message = nullptr;

    explicit operator bool() const noexcept {
        return error;
    }
};

} // namespace rayalto::utils::misc

#endif // RA_UTILS_RAUTILS_MISC_STATUS_H_
