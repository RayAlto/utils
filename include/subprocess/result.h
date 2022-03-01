#ifndef RA_UTILS_SUBPROCESS_RESULT_HPP_
#define RA_UTILS_SUBPROCESS_RESULT_HPP_

#include <string>

namespace rayalto {
namespace utils {
namespace subprocess {

struct Result {
    bool exit_normally = true;
    int exit_code = 0;
    std::string stdout;
    std::string stderr;
};

} // namespace subprocess
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_SUBPROCESS_RESULT_HPP_
