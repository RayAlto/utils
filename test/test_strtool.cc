#include "string/strtool.h"

#include <iostream>
#include <string>

using namespace rayalto::utils;

int main(int argc, char const* argv[]) {
    bool foo = string::compare_ic("abc", "ABC");
    bool bar = string::compare_ic("def", "ABC");
    std::cout << std::boolalpha << foo << std::endl << bar << std::endl;
    return 0;
}
