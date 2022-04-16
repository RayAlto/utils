#include "string/strtool.h"

#include <iostream>
#include <string>
#include <vector>

using namespace rayalto::utils;

int main(int argc, char const* argv[]) {
    std::vector<std::string> strings {"foo", "bar", "hello", "world"};
    std::cout
        << string::join(", ", {"foo", "bar", "hello", "world"}) << std::endl
        << string::join(", ", strings.begin(), strings.end()) << std::endl
        << string::random_string() << std::endl
        << string::hex_string("灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！")
        << std::endl
        << string::data_string(string::parse_hex(
               "e7818ce6b3a8e6b0b8e99b8fe5a194e88fb2e596b5efbc8ce7818ce6b3a8e6b0b8e99b8fe5a194e88fb2e8b0a2e8b0a2e596b5efbc81"))
        << std::endl;

    return 0;
}
