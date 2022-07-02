#include <chrono>
#include <cstdio>
#include <thread>

#include "rautils/misc/thread_id.h"

int main(int /* argc */, char const* /* argv */[]) {
    for (int i = 0; i < 100; ++i) {
        std::thread t([&]() -> void {
            std::printf("%d\n", rayalto::utils::misc::thread_id());
            std::this_thread::sleep_for(std::chrono::milliseconds(90));
        });
        t.detach();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
