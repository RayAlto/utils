#include <iostream>

#include "rautils/misc/mime_types.h"

using namespace rayalto::utils;

int main(int argc, char const *argv[]) {
    std::cout << std::boolalpha << misc::MimeTypes::know("json") << std::endl;
    std::cout << misc::MimeTypes::get("png") << std::endl;
    std::cout << misc::MimeTypes::default_text << std::endl;
    std::cout << misc::MimeTypes::default_binary << std::endl;
    return 0;
}
