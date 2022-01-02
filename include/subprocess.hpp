#ifndef RA_UTILS_INCLUDE_SUBPROCESS_HPP_
#define RA_UTILS_INCLUDE_SUBPROCESS_HPP_

#include <string>
#include <vector>

#include "args.hpp"
#include "result.hpp"

namespace rayalto {
namespace utils {

/**
 * Run a command and get its stdout, stderr and exit code
 * Example:
 *      Subprocess subprocess({"ls", "-ahl"});
 *      subprocess.run();
 *      subprocess::Result result = subprocess.result();
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
    explicit Subprocess(const subprocess::Args& command_args);
    explicit Subprocess(const Subprocess& subprocess) = delete;

    Subprocess& operator=(const Subprocess&) = delete;

    ~Subprocess();

    // Run the specified command and return the result
    subprocess::Result run(bool capture_output = true);

    subprocess::Args args() const;
    subprocess::Result result() const;

    // Run a command directly and get the result
    static subprocess::Result run(const subprocess::Args& args,
                                  bool capture_output = true);
    static subprocess::Result run(const char* arg, bool capture_output = true);

private:
    subprocess::Result result_;
    subprocess::Args args_;
};

} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_INCLUDE_SUBPROCESS_HPP_
