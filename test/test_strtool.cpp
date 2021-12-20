#include "strtool.hpp"

#include <iostream>
#include <string>

using namespace rayalto::utils;

int main(int argc, char const* argv[]) {
    std::string foo {"127.0.0.114514"};
    for (auto i : string::split(foo, '.', [](std::string& line) -> bool {
             if (line == "114514") {
                 return false;
             }
             return true;
         })) {
        std::cout << i << std::endl;
    }
    return 0;
}
