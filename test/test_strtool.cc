#include "string/strtool.h"

#include <iostream>
#include <string>
#include <vector>

using namespace rayalto;

int main(int argc, char const* argv[]) {
    std::vector<std::string> strings {"foo", "bar", "hello", "world"};
    std::cout << utils::string::join(", ", {"foo", "bar", "hello", "world"})
              << std::endl
              << utils::string::join(", ", strings.begin(), strings.end())
              << std::endl
              << utils::string::random_string() << std::endl;

    return 0;
}
