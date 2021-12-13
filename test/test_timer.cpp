#include <chrono>
#include <iostream>

#include "ra_utils.hpp"

using rayalto::utils::Timer;

int main(int argc, char const *argv[]) {
    Timer t([&]() { std::cout << "hello" << std::endl; },
            std::chrono::milliseconds(100));
    t.start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    t.stop();
    t.set_interval(std::chrono::seconds(1));
    t.start();
    std::this_thread::sleep_for(std::chrono::seconds(2)
                                + std::chrono::milliseconds(100));
    t.stop();
    return 0;
}
