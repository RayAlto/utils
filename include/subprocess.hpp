#ifndef RA_UTILS_INCLUDE_SUBPROCESS_HPP_
#define RA_UTILS_INCLUDE_SUBPROCESS_HPP_

#include <string>
#include <vector>

#include "result.hpp"

namespace rayalto {
namespace utils {
namespace subprocess {

using Args = std::vector<std::string>;

/**
 * Run a command and get its stdout, stderr and exit code
 * Example:
 *      Subprocess subprocess({"ls", "-ahl"});
 *      subprocess.run();
 *      Result result = subprocess.result();
 *      if (result.exit_normally()) {
 *          std::cout << result.stdout << '\n';
 *      }
 *      else {
 *          std::ceer << "error(" << result.exit_code << "):"
 *                    << result.stderr << '\n';
 *      }
 */
class Subprocess {
public:
    explicit Subprocess(const Args& command_args);
    explicit Subprocess(const Subprocess& subprocess) = delete;

    Subprocess& operator=(const Subprocess&) = delete;

    ~Subprocess();

    // Run the specified command and return the result
    Result run(bool capture_output = true);

    Args args() const;
    Result result() const;

    // Run a command directly and get the result
    static Result run(const Args& args, bool capture_output = true);
    static Result run(const char* arg, bool capture_output = true);

private:
    Result _result;
    Args _args;
};

} // namespace subprocess
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_INCLUDE_SUBPROCESS_HPP_
