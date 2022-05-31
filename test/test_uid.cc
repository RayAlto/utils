#include <iostream>

#include "rautils/misc/uid.h"

using namespace rayalto::utils;

int main(int argc, char const* argv[]) {
    misc::Uid id("ffff::ffff");
    std::cout << id.to_string(false, '0') << std::endl;
    ++id;
    std::cout << id.to_string(false, '0') << std::endl;
    id += 0xffff;
    std::cout << id.to_string(false, '0') << std::endl;
    std::cout << std::boolalpha << (id > misc::Uid("ffff::ffff")) << std::endl;
    std::cout << std::boolalpha << (id == misc::Uid("ffff::1:ffff"))
              << std::endl;
    return 0;
}
