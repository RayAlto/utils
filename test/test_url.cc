#include <iostream>

#include "rautils/network/general/url.h"

using rayalto::utils::network::general::Url;

int main(int /* argc */, char const* /* argv */[]) {
    Url url("https://www.google.com:443/search?q=url");
    std::cout << url.to_string() << std::endl;
    return 0;
}
