#include <iostream>
#include <string>

#include "request/mime_parts.h"
#include "request/request.h"
#include "util/mime_types.h"

using rayalto::utils::util::MimeTypes;
using rayalto::utils::Request;
using namespace rayalto::utils::request;

int main(int argc, char const *argv[]) {
    Request request;
    // clang-format off
    request.url("https://httpbin.org/anything")
           .method(Method::POST)
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
                 /* part data */ MimePart()
                     .is_file(true)
                     .data(/* local file name */"example.png")
                     .file_name(/* remote file name */"senpai.png")
                     .type(MimeTypes::get("png"))
                },
                {/* part name */ "data",
                 /* part data */ MimePart()
                     .data(R"+*({"kimochi": "1919810", "come": "114514"})+*")
                     .type(MimeTypes::get("json"))
                }
           })
           .request();
    // clang-format on
    /* request.body(R"+*+*({"age": 114514, "role": "student"})+*+*", */
    /*              MimeTypes::get("json")); */
    Response response = request.response();
    std::cout << response.body << std::endl;
    return 0;
}
