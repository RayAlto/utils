# RA-Utils

RayAlto's Stupid Self-use Noob Utils

## How To

### 1. String Tool

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

### 2. MIME Type

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

### 3. Request

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

### 4. WebSocket

A stupid WebSocket client, thanks to [zaphoyd (Peter Thorson)](https://github.com/zaphoyd) and his great work [websocketpp](https://github.com/zaphoyd/websocketpp) which is much more simple to use than the self-proclaimed "simple-to-use" libwebsockets.

Because the client is multi-threaded, it takes a certain amount of time for the client to actually connect to server after `url()` called. You can call `connected()` to check whether client is really connected to server.

```c++
#include <chrono>
#include <cstdint>
#include <iostream>
#include <system_error>
#include <thread>

#include "websocket/ws_client.h"

using namespace rayalto::utils;
using websocket::CloseStatus;
using websocket::MessageType;
using websocket::close_status::Code;

int main(int argc, char const* argv[]) {
    // error
    std::error_code ws_error;

    // default close reason
    WsClient::default_close_status.reason = "shut up";
    WsClient client;

    client.on_receive([&](WsClient& client,
                          const MessageType& type,
                          const std::string& message) -> void {
        std::cout << "receive: " << message << std::endl;
    });

    // connection established
    client.on_establish([&](WsClient& client) -> void {
        std::cout << "connected: " << client.url() << std::endl;
    });

    // connection failed
    client.on_fail(
        [&](WsClient& client, const CloseStatus& local_status) -> void {
            std::cout << "failed to connect (" << local_status.code
                      << "): " << local_status.status << std::endl;
        });

    // connection closed
    client.on_close(
        [&](WsClient& client, const CloseStatus& remote_status) -> void {
            std::cout << "closed (" << remote_status.code
                      << "): " << remote_status.status << ", "
                      << remote_status.reason << std::endl;
        });

    // local echo server
    client.url("ws://127.0.0.1:8080");
    client.connect(ws_error);

    if (ws_error) {
        std::cout << "error get connection: " << ws_error.message()
                  << std::endl;
    }

    else {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // say hello
        client.send(MessageType::TEXT, "hello", ws_error);
        std::this_thread::sleep_for(std::chrono::seconds(2));

        if (ws_error) {
            std::cout << "error send message: " << ws_error.message()
                      << std::endl;
        }

        // fuck off
        client.disconnect(CloseStatus(Code::NORMAL, "fuck off"));
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // my echo server with tls, DO NOT mess around
    client.url("wss://test.rayalto.top:8443");
    client.connect(ws_error);

    if (ws_error) {
        std::cout << "error get connection: " << ws_error.message()
                  << std::endl;
    }

    else {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // say hello
        client.send(MessageType::TEXT, "hello", ws_error);

        if (ws_error) {
            std::cout << "error send message: " << ws_error.message()
                      << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    return 0;
    // did not call disconnect, default status will be sent
}
```

output

```plain
connected: ws://127.0.0.1:8080
receive: Request served by RayAltoNUC
receive: hello
closed (1000): Normal close,
connected: wss://test.rayalto.top:8443
receive: hello
closed (1000): Normal close, shut up
```

### 5. Crypto

Encode & decode, hash, random byte, ... stuff

```c++
#include <iostream>

#include "crypto/codec.h"
#include "crypto/diffie_hellman.h"
#include "crypto/hash.h"
#include "crypto/random.h"
#include "crypto/encrypt.h"

#include "string/strtool.h"

using namespace rayalto::utils;

int main(int argc, char const *argv[]) {
    // echo 'I am nearly dead\n' | openssl dgst -sha256
    std::cout << string::hex_string(
        crypto::hash::sha256("I am nearly dead\n"))
              << std::endl;

    // echo 'I am nearly dead\n' | openssl enc -d -base64
    std::cout << string::data_string(
        crypto::codec::base64_encode("I am nearly dead\n"))
              << std::endl;

    // openssl rand -hex 4
    std::cout << string::hex_string(crypto::random::bytes(4)) << std::endl;

    // openssl enc -e -aes256cbc
    std::vector<unsigned char> encrypted =
        crypto::encrypt::aes256cbc("I am nearly dead",
                                   54,
                                   "01234567890123456789012345678901",
                                   "0123456789012345");
    std::cout << string::hex_string(encrypted) << std::endl;

    // openssl enc -d -aes256cbc
    std::vector<unsigned char> decrypted = crypto::decrypt::aes256cbc(
        encrypted.data(),
        encrypted.size(),
        reinterpret_cast<const unsigned char *>(
            "01234567890123456789012345678901"),
        reinterpret_cast<const unsigned char *>("0123456789012345"));
    std::cout << string::data_string(decrypted) << std::endl;

    // Diffie Hellman key exchange
    std::vector<unsigned char> public_key_b64 = string::to_data(
        "AAABAHJuXLnNjrPxIFRN9TL77ML0KSL6iCDRGYPseiyYIahJRgMMrI8Umn5R9zP9gzaM6bpTObK/rOEy6fClj3H5udKDqVsU/71qIf8oo1KadvqwYMz1OpKiFTQlbdarEwbeGjL/C+eZD22kD4xG5VDZFz8tAt2+HMe2waxNna/ERyx1TlOyur7dBSjb0Wlg9kk2R3qufBKSzVz6TWoB+tzDc0wciMEXB6iu5aZutUyd0S9IGoawl/xTSQay2NPDjKIQnnwpj5kYFGVLXAexcpjFH0NedDE/zddM093sW+6wHSsDFfykYYvCqY0VwlawB8N63sAZB50oARyuV2OUQiT4RMQ=");
    std::vector<unsigned char> param_p_b64 = string::to_data(
        "AAABAQCInR/DefsGtJ2uECMbdDVaEOJRB+6cn86I+35udWrHcVzdhiElnMRuSWLdEkTahdJ7IG5k0qKyP02RFch432Km74QOosGsbiMm6XwMoXmH5TwI8wV96+MWhySYoRDrNHbx0ZbQOuLjiiSTh3Ccim/aXNymq4WIvxERKIy7Hg8SRyk3AeR8BoV3eR1ySKRCsGYKNmh1OGtGenyIeAQKkHYqStnivwnodXaCiHSUA/PbFi5Ng+G06JTyGkVHmkduuuTNjsmCB/YjtaGk0BKraQPdcZXsos+cq93S7ajC+ej/ud40OMNFmfXCeh3KuT50AVQPfAlChqOZYQD/P+eV5ofT");
    std::vector<unsigned char> param_q_b64 =
        string::to_data("AAAAIQCHUyuspW42aIvMdWW/srwsY4Hb4PMWg1se3Iwq86zGsQ==");
    std::vector<unsigned char> param_g_b64 = string::to_data(
        "AAABAFOW0YHLmXsnt/PUKIHEuyONMk7URML9idRXsZLrmTUyMxwVWov1h93O4ep5Ue50G5aZ1qo4GbL8aK8Fgp1FMRm4E6zWIp6NDom7+BY3nDZF5TG09d2w8ZzChlH1E1giLacv1riG6WvbS6MWpV4QvQ/B9Pp8ySyeEDCWOcAN7sjcQu7TNj+CfHmYGNFlpMuye8s3CiWz6ocaZ8aj7kiqpN+jORQzgLyQKfd+C6Ai/3mYaDJpF3mBsYrqjsoarX/q8yZnPfTxia/RSnnZZzTTb/JPiOivoPWwWiC9X0IIXwXFpgxy8yWk83xVIwAGPuPG2i1+Z4vyv0k/p9KvHX+dUs4=");
    crypto::DiffieHellman foo;
    foo.param_p(crypto::codec::base64_decode(param_p_b64));
    foo.param_q(crypto::codec::base64_decode(param_q_b64));
    foo.param_g(crypto::codec::base64_decode(param_g_b64));
    foo.peer_public_key(crypto::codec::base64_decode(public_key_b64));
    foo.generate_key_pair();
    foo.derive_shared_key();
    std::cout << "===== shared key" << foo.shared_key().size()
              << " =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.shared_key()))
              << std::endl;
    std::cout << "===== public key =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.public_key()))
              << std::endl;
    std::cout << "===== private key =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.private_key()))
              << std::endl;
    std::cout << "===== param p =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.param_p()))
              << std::endl;
    std::cout << "===== param q =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.param_q()))
              << std::endl;
    std::cout << "===== param g =====" << std::endl;
    std::cout << string::data_string(
        crypto::codec::base64_encode(foo.param_g()))
              << std::endl;

    return 0;
}
```
