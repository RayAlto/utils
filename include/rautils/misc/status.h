#ifndef RA_UTILS_RAUTILS_MISC_STATUS_H_
#define RA_UTILS_RAUTILS_MISC_STATUS_H_

#include <memory>
#include <string>

namespace rayalto {
namespace utils {
namespace misc {

struct Status {
    bool error = false;
    std::unique_ptr<std::string> message = nullptr;

    operator bool() const noexcept {
        return error;
    }

    operator std::string() const noexcept {
        if (message == nullptr) {
            return {};
        }
        else {
            return *message;
        }
    }
};

} // namespace misc
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_MISC_STATUS_H_
