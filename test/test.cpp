#include <iostream>

#include "ra_utils.hpp"

using rayalto::utils::subprocess::Result;
using rayalto::utils::subprocess::Subprocess;

int main(int argc, char const *argv[]) {
    Result result = Subprocess::run({"ls", "-ahl", "/usr/bin"});
    std::cout << "stdout:\n"
              << result.stdout // stdout
              << "stderr:\n"
              << result.stderr // stderr
              << "exit_code:" << result.exit_code << '\n';
    return 0;
}
