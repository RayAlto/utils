#include <iostream>
#include <string>

#include "request/request.h"
#include "util/mime_types.h"

using rayalto::utils::util::MimeTypes;
using rayalto::utils::Request;
using namespace rayalto::utils::request;

int main(int argc, char const *argv[]) {
    Request request;
    request.url("https://httpbin.org/anything");
    request.method(Method::POST);
    request.useragent("RayAlto/114514");
    request.header({{"foo", "bar"}, {"114514", "1919810"}});
    request.cookie({{"brand", "Nabisco Oreo"}, {"comment", "delicious"}});
    /* request.body(R"+*+*({"age": 114514, "role": "student"})+*+*", */
    /*              MimeTypes::get("json")); */
    // clang-format off
    request.mime_parts().add({
        {/* part name */ "file",
         /* part data */ MimePart()
              .is_file(true)
              .data("example.png")
              .file_name("senpai.png")
              .type(MimeTypes::get("png"))
        },
        {/* part name */ "data",
         /* part data */ MimePart()
              .data(R"+*+*({"kimochi": "1919810", "come": "114514"})+*+*")
              .type(MimeTypes::get("json"))
        }
    });
    // clang-format on
    request.request();
    Response response = request.response();
    std::cout << response.body << std::endl;
    return 0;
}
