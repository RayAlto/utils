#include <iostream>

#include "ra_utils.hpp"

using rayalto::utils::subprocess::Result;
using rayalto::utils::Subprocess;

int main(int argc, char const *argv[]) {
    Result result = Subprocess::run({"lp", "-ahl", "/usr/bin"});
    std::cout << "stdout:\n"
              << result.stdout // stdout
              << "stderr:\n"
              << result.stderr // stderr
              << "exit_code:" << result.exit_code << '\n';
    return 0;
}
