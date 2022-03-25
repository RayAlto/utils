# RA-Utils

RayAlto's Stupid Self-use Noob Utils

## How To

### 1. Subprocess

Run a command and get its stdout, stderr and exit_code. Base on `exec()` family
of functions. Example:

```c++
using rayalto::util::subprocess::Result;
using rayalto::util::Subprocess;
// ...

Result result = Subprocess::run({"ls", "-ahl", "/usr/bin"});
std::cout << result.stdout << std::endl;
```

- It's **not** a shell, it will **not** expand `~` or behave like a shell.

### 2. Timer

Call a void function at specified intervals

```c++
using rayalto::util::Timer;
// ...

Timer timer([]() {
    std::cout << "114514" << std::endl;
}, std::chrono::milliseconds(1919810));
timer.start();
std::this_thread::sleep_for(std::chrono::milliseconds(1145141919810));
timer.set_interval(std::chrono::seconds(1)
                   + std::chrono::milliseconds(114)
                   + std::chrono::milliseconds(514));
std::this_thread::sleep_for(std::chrono::milliseconds(1145141919810));
timer.stop();
```

### 3. String Tool

Some tools for string, for example:

```c++
/**
 * Split "127.0.0.114514" with '.' and get rid of "114514"
 */

using namespace rayalto::utils;

// ...

std::string foo {"127.0.0.114514"};
for (auto i : string::split(foo, '.', [](std::string& line) -> bool {
            if (line == "114514") {
                return false;
            }
            return true;
        })) {
    std::cout << i << std::endl;
}
```

output:

```plain
127
0
0
```

### 4. MIME Type

Get MIME Type from file.

```c++
std::cout << std::boolalpha << util::MimeTypes::know("foo.png") << std::endl;
std::cout << util::MimeTypes::get("foo.png") << std::endl;
std::cout << std::boolalpha << util::MimeTypes::know("foo.wtf") << std::endl;
std::cout << util::MimeTypes::get("foo.wtf") << std::endl;
std::cout << util::MimeTypes::get("foo.wtf", "application/what-the-fuck") << std::endl;
std::cout << util::MimeTypes::default_text << std::endl;
std::cout << util::MimeTypes::default_binary << std::endl;
```

output:

```plain
true
image/png
false
application/octet-stream
application/what-the-fuck
text/plain
application/octet-stream
```

> You can update the "[database](./src/util/mime_types_data.cc)" by running [this python script](./script/update_mime_types_data.py). The script will download & parse MIME Types from:
>
> - [Apache HTTPD](https://svn.apache.org/repos/asf/httpd/httpd/trunk/docs/conf/mime.types)
> - [Nginx](https://hg.nginx.org/nginx/raw-file/default/conf/mime.types)

### 5. Request

A stupid curl wrapper

```c++
using rayalto::utils::util::MimeTypes;
using rayalto::utils::Request;
using namespace rayalto::utils::request;

Request request;
request.url("https://httpbin.org/anything");
request.method(Method::POST);
request.useragent("RayAlto/114514");
request.header({
    {"foo", "bar"},
    {"114514", "1919810"}
});
request.cookie({
    {"brand", "Nabisco Oreo"},
    {"comment", "delicious"}
});
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
request.request();
Response response = request.response();
std::cout << response.body << std::endl;
```

output

```plain
{
  "args": {},
  "data": "",
  "files": {
    "file": "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAOElEQVQ4y2NgoBAw4pH7jyT/n0S9KJr+45NnotQLw8
AAFiJjajQWiDDg/4C5gBFLqBMjR1Q0EgUAOTcME3wmR8oAAAAASUVORK5CYII="
  },
  "form": {
    "data": "{\"kimochi\": \"1919810\", \"come\": \"114514\"}"
  },
  "headers": {
    "114514": "1919810",
    "Accept": "*/*",
    "Content-Length": "465",
    "Content-Type": "multipart/form-data; boundary=------------------------40e411258b65ef2a",
    "Cookie": "brand=Nabisco Oreo; comment=delicious;",
    "Foo": "bar",
    "Host": "httpbin.org",
    "User-Agent": "RayAlto/114514",
    "X-Amzn-Trace-Id": "Root=1-6232a7a2-7a4070915e0907a528fa2dd8"
  },
  "json": null,
  "method": "POST",
  "origin": "36.161.120.3",
  "url": "https://httpbin.org/anything"
}
```

### 6. WebSocket

A stupid WebSocket client, thanks to [zaphoyd (Peter Thorson)](https://github.com/zaphoyd) and his great work [websocketpp](https://github.com/zaphoyd/websocketpp) which is much more simple to use than the self-proclaimed "simple-to-use" libwebsockets.

Because the client is multi-threaded, it takes a certain amount of time for the client to actually connect to server after `url()` called. You can call `connected()` to check whether client is really connected to server.

```c++
#include <chrono>
#include <iostream>
#include <thread>

#include "websocket/ws_client.h"

using rayalto::utils::websocket::MessageType;
using rayalto::utils::WsClient;

int main(int argc, char const* argv[]) {
    WsClient client;
    client.on_receive(
        [&](const MessageType& type, const std::string& message) -> void {
            std::cout << message << std::endl;
        });
    // local echo server
    client.url("ws://127.0.0.1:8080");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    client.send(MessageType::TEXT, "hello");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // my echo server with tls, DO NOT mess around
    client.url("wss://test.rayalto.top:8443");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    client.send(MessageType::TEXT, "hello");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}
```

output

```plain
hello
hello
```
