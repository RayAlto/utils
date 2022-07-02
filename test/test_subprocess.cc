#include <iostream>

#include "rautils/system/subprocess.h"

using rayalto::utils::system::Subprocess;

int main(int /* argc */, char const* /* argv */[]) {
    Subprocess s;
    s.args(Subprocess::Args {"echo", "hello world"}).run();
    std::cout << "exit status: " << s.get_exit_status()
              << "\nstdout: " << s.get_stdout()
              << "\nstderr: " << s.get_stderr() << std::endl;
    return 0;
}
