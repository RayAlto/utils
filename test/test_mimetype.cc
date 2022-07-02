#include <iostream>

#include "rautils/misc/mime_types.h"

using rayalto::utils::misc::MimeTypes;

int main(int /* argc */, char const* /* argv */[]) {
    std::cout << std::boolalpha << MimeTypes::know("json") << std::endl;
    std::cout << MimeTypes::get("png") << std::endl;
    std::cout << MimeTypes::default_text << std::endl;
    std::cout << MimeTypes::default_binary << std::endl;
    return 0;
}
