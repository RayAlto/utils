#include <iostream>

#include "util/mime_types.h"

using namespace rayalto::utils;

int main(int argc, char const *argv[]) {
    std::cout << std::boolalpha << util::MimeTypes::know("json") << std::endl;
    std::cout << util::MimeTypes::get("png") << std::endl;
    std::cout << util::MimeTypes::default_text << std::endl;
    std::cout << util::MimeTypes::default_binary << std::endl;
    return 0;
}
