#include <chrono>
#include <iostream>
#include <vector>

#include "ra_utils.hpp"

using rayalto::utils::subprocess::Result;
using rayalto::utils::Subprocess;
using rayalto::utils::Timer;

int main(int argc, char const *argv[]) {
    Timer t([&]() { std::cout << "hello" << std::endl; },
            std::chrono::milliseconds(100));
    t.start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    t.set_interval(std::chrono::seconds(1));
    std::this_thread::sleep_for(std::chrono::seconds(4)
                                + std::chrono::milliseconds(100));
    t.stop();
    return 0;
}
