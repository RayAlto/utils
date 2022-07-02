#include <iostream>
#include <string>

#include "rautils/misc/mime_types.h"
#include "rautils/network/general/url.h"
#include "rautils/network/request.h"

using rayalto::utils::misc::MimeTypes;
using rayalto::utils::network::Request;
using rayalto::utils::network::general::Url;

int main(int /* argc */, char const* /* argv */[]) {
    Request request;
    // clang-format off
    request.url(Url("https://httpbin.org/anything"))
           .method(Request::Method::POST)
           .useragent("RayAlto/114514")
           .header({
                {"foo",    "bar"    },
                {"114514", "1919810"}
           })
           .cookie({
                {"brand",   "Nabisco Oreo"},
                {"comment", "delicious"   }
           })
           .mime_parts({
                {/* part name */ "file",
                /* part data */ Request::MimePart()
                     .is_file(true)
                     .data(/* local file name */"example.png")
                     .file_name(/* remote file name */"senpai.png")
                     .type(MimeTypes::get("png"))
                },
                {/* part name */ "data",
                /* part data */ Request::MimePart()
                     .data(R"+*({"kimochi": "1919810", "come": "114514"})+*")
                     .type(MimeTypes::get("json"))
                }
           })
           .request();
    // clang-format on
    /* request.body(R"+*+*({"age": 114514, "role": "student"})+*+*", */
    /*              MimeTypes::get("json")); */
    const Request::Response& response = *request.response();
    std::cout << response.body << std::endl;
    return 0;
}
