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
std::cout << std::boolalpha << MimeTypes::know("foo.png") << std::endl;
std::cout << MimeTypes::get("foo.png") << std::endl;
std::cout << std::boolalpha << MimeTypes::know("foo.wtf") << std::endl;
std::cout << MimeTypes::get("foo.wtf") << std::endl;
std::cout << MimeTypes::get("foo.wtf", "application/what-the-fuck") << std::endl;
std::cout << MimeTypes::default_text << std::endl;
std::cout << MimeTypes::default_binary << std::endl;
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
#include <iostream>
#include <string>

#include "rautils/network/request.h"
#include "rautils/misc/mime_types.h"

using rayalto::utils::misc::MimeTypes;
using rayalto::utils::network::Request;

int main(int argc, char const *argv[]) {
    Request request;
    // clang-format off
    request.url("https://httpbin.org/anything")
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
    Request::Response response = request.response();
    std::cout << response.body << std::endl;
    return 0;
}
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
#include <iostream>
#include <system_error>
#include <thread>

#include "rautils/network/ws_client.h"

using rayalto::utils::network::WsClient;

int main(int argc, char const* argv[]) {
    // error
    std::error_code ws_error;

    // default close reason
    WsClient::default_close_status().reason = "shut up";
    WsClient client;

    client.on_receive([&](WsClient& client,
                          const WsClient::MessageType& type,
                          const std::string& message) -> void {
        std::cout << "receive: " << message << std::endl;
    });

    // connection established
    client.on_establish([&](WsClient& client) -> void {
        std::cout << "connected: " << client.url() << std::endl;
    });

    // connection failed
    client.on_fail([&](WsClient& client,
                       const WsClient::CloseStatus& local_status) -> void {
        std::cout << "failed to connect (" << local_status.code
                  << "): " << local_status.status << std::endl;
    });

    // connection closed
    client.on_close([&](WsClient& client,
                        const WsClient::CloseStatus& remote_status) -> void {
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
        client.send(WsClient::MessageType::TEXT, "hello", ws_error);
        std::this_thread::sleep_for(std::chrono::seconds(2));

        if (ws_error) {
            std::cout << "error send message: " << ws_error.message()
                      << std::endl;
        }

        // fuck off
        client.disconnect(WsClient::CloseStatus(
            WsClient::CloseStatus::Code::NORMAL, "fuck off"));
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
        client.send(WsClient::MessageType::TEXT, "hello", ws_error);

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

#include "rautils/crypto/codec.h"
#include "rautils/crypto/diffie_hellman.h"
#include "rautils/crypto/hash.h"
#include "rautils/crypto/random.h"
#include "rautils/crypto/encrypt.h"
#include "rautils/string/strtool.h"

using namespace rayalto::utils::crypto;
using rayalto::utils::string::hex_string;
using rayalto::utils::string::data_string;
using rayalto::utils::string::to_data;

int main(int argc, char const *argv[]) {
    // echo '灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n' | openssl dgst -sha256
    std::cout << hex_string(
        hash::sha256("灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n"))
              << std::endl;

    // echo '灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n' | openssl enc -d -base64
    std::cout << data_string(
        codec::base64_encode("灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！\n"))
              << std::endl;

    std::cout << hex_string(random::bytes(4)) << std::endl;

    std::vector<unsigned char> encrypted =
        encrypt::aes256cbc("灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！",
                           54,
                           "01234567890123456789012345678901",
                           "0123456789012345");
    std::cout << hex_string(encrypted) << std::endl;

    std::vector<unsigned char> decrypted = decrypt::aes256cbc(
        encrypted.data(),
        encrypted.size(),
        reinterpret_cast<const unsigned char *>(
            "01234567890123456789012345678901"),
        reinterpret_cast<const unsigned char *>("0123456789012345"));
    std::cout << data_string(decrypted) << std::endl;

    std::vector<unsigned char> public_key_b64 = to_data(
        "AAABAHJuXLnNjrPxIFRN9TL77ML0KSL6iCDRGYPseiyYIahJRgMMrI8Umn5R9zP9gzaM6bpTObK/rOEy6fClj3H5udKDqVsU/71qIf8oo1KadvqwYMz1OpKiFTQlbdarEwbeGjL/C+eZD22kD4xG5VDZFz8tAt2+HMe2waxNna/ERyx1TlOyur7dBSjb0Wlg9kk2R3qufBKSzVz6TWoB+tzDc0wciMEXB6iu5aZutUyd0S9IGoawl/xTSQay2NPDjKIQnnwpj5kYFGVLXAexcpjFH0NedDE/zddM093sW+6wHSsDFfykYYvCqY0VwlawB8N63sAZB50oARyuV2OUQiT4RMQ=");
    std::vector<unsigned char> param_p_b64 = to_data(
        "AAABAQCInR/DefsGtJ2uECMbdDVaEOJRB+6cn86I+35udWrHcVzdhiElnMRuSWLdEkTahdJ7IG5k0qKyP02RFch432Km74QOosGsbiMm6XwMoXmH5TwI8wV96+MWhySYoRDrNHbx0ZbQOuLjiiSTh3Ccim/aXNymq4WIvxERKIy7Hg8SRyk3AeR8BoV3eR1ySKRCsGYKNmh1OGtGenyIeAQKkHYqStnivwnodXaCiHSUA/PbFi5Ng+G06JTyGkVHmkduuuTNjsmCB/YjtaGk0BKraQPdcZXsos+cq93S7ajC+ej/ud40OMNFmfXCeh3KuT50AVQPfAlChqOZYQD/P+eV5ofT");
    std::vector<unsigned char> param_q_b64 =
        to_data("AAAAIQCHUyuspW42aIvMdWW/srwsY4Hb4PMWg1se3Iwq86zGsQ==");
    std::vector<unsigned char> param_g_b64 = to_data(
        "AAABAFOW0YHLmXsnt/PUKIHEuyONMk7URML9idRXsZLrmTUyMxwVWov1h93O4ep5Ue50G5aZ1qo4GbL8aK8Fgp1FMRm4E6zWIp6NDom7+BY3nDZF5TG09d2w8ZzChlH1E1giLacv1riG6WvbS6MWpV4QvQ/B9Pp8ySyeEDCWOcAN7sjcQu7TNj+CfHmYGNFlpMuye8s3CiWz6ocaZ8aj7kiqpN+jORQzgLyQKfd+C6Ai/3mYaDJpF3mBsYrqjsoarX/q8yZnPfTxia/RSnnZZzTTb/JPiOivoPWwWiC9X0IIXwXFpgxy8yWk83xVIwAGPuPG2i1+Z4vyv0k/p9KvHX+dUs4=");
    DiffieHellman foo;
    foo.param_p(codec::base64_decode(param_p_b64));
    foo.param_q(codec::base64_decode(param_q_b64));
    foo.param_g(codec::base64_decode(param_g_b64));
    foo.peer_public_key(codec::base64_decode(public_key_b64));
    foo.generate_key_pair();
    foo.derive_shared_key();
    std::cout << "===== shared key" << foo.shared_key().size()
              << " =====" << std::endl;
    std::cout << data_string(codec::base64_encode(foo.shared_key()))
              << std::endl;
    std::cout << "===== public key =====" << std::endl;
    std::cout << data_string(codec::base64_encode(foo.public_key()))
              << std::endl;
    std::cout << "===== private key =====" << std::endl;
    std::cout << data_string(codec::base64_encode(foo.private_key()))
              << std::endl;
    std::cout << "===== param p =====" << std::endl;
    std::cout << data_string(codec::base64_encode(foo.param_p())) << std::endl;
    std::cout << "===== param q =====" << std::endl;
    std::cout << data_string(codec::base64_encode(foo.param_q())) << std::endl;
    std::cout << "===== param g =====" << std::endl;
    std::cout << data_string(codec::base64_encode(foo.param_g())) << std::endl;

    return 0;
}
```

output:

```plain
04b261ea1b78cb5fb4407a03b88e00a1b545075f6b52cf2c753b0c45cceeed8d
54GM5rOo5rC46ZuP5aGU6I+y5Za177yM54GM5rOo5rC46ZuP5aGU6I+y6LCi6LCi5Za177yBCg==
6f551245
6abd67bbe343665df02a67f70fd9fabfacb79e01c624622b4ec43c219a27b54a02ac8b29c1754a105ca82e9bf52ee6f4d66591c6a55c835109fdc527ad85b
4cb
灌注永雏塔菲喵，灌注永雏塔菲谢谢喵！
===== shared key256 =====
CVhm455+C6ylOP4UgRAk8F5VRkRlM8K8/ujPnpf4P6R2RlDstJAjlW2SoxY0IvK4VwwIx8PaoxWrL9HWDn/jqhdgK6EpbqLDmQI4O1b5iAyVnp8w7qrJlgQQbyZgiPYrnnkXZvEmKUlr/klzJ1VinxmqAC2XlInTT2KqDoPiz3+1Nn8BVILqhIdFjr9Aq3v7l7v/en8n4OnYHFH8JlQBdM2KLblYxKtktx6HIuLaCEEuWJu4PdhcmrRd70dbqdd+gTBG70Sx81qFPV8x2oAxFSl65LQb3yxx/hPpmxDNBfmt+E8ud+lPvwmP3tMkJuQc2GSgsSlfnDodfJsFV4YJNw==
===== public key =====
AAABAFT62/ivadDkzWcPwQxLOs3bWCex16Wj96FTlVjMMQiIm633au8Y64UlJ/E/lS344MNN8V43FDHYOqkbLy98ftt4CHjnvnB7Qb56Dnl6oLubSMYV0NvWDA06I0vwJm5/T2mZ9Zk2CRfM8eDS0psLtBk+WrJFClomJgucZv9Sb6eTVNQWn3/vJTJx14sH0MK29sqb1frFHZwL8VWxmPV/usAEYfoexTuC/y8VTaypaMnzR/z+gXSeL5bMXeLvjIsUvTT6bCkE1bBLRaVDkCD0gaLCm8wIKwkG8lRNSs/nWvg4Xe8RKt57rCubddn0U8Hi2zUs3jdjQRkL7zormxSJtxY=
===== private key =====
AAAAIEApv5wXaCVwKbZZge8oxQT7lKPAROcXZDOev/IxLgQa/X8AAIOfFbfIVQAAAAABAFT62/ivadDkzWcPwQxLOs3bWCex16Wj96FTlVjMMQiI
===== param p =====
AAABAQCInR/DefsGtJ2uECMbdDVaEOJRB+6cn86I+35udWrHcVzdhiElnMRuSWLdEkTahdJ7IG5k0qKyP02RFch432Km74QOosGsbiMm6XwMoXmH5TwI8wV96+MWhySYoRDrNHbx0ZbQOuLjiiSTh3Ccim/aXNymq4WIvxERKIy7Hg8SRyk3AeR8BoV3eR1ySKRCsGYKNmh1OGtGenyIeAQKkHYqStnivwnodXaCiHSUA/PbFi5Ng+G06JTyGkVHmkduuuTNjsmCB/YjtaGk0BKraQPdcZXsos+cq93S7ajC+ej/ud40OMNFmfXCeh3KuT50AVQPfAlChqOZYQD/P+eV5ofT
===== param q =====
AAAAIQCHUyuspW42aIvMdWW/srwsY4Hb4PMWg1se3Iwq86zGsQ==
===== param g =====
AAABAFOW0YHLmXsnt/PUKIHEuyONMk7URML9idRXsZLrmTUyMxwVWov1h93O4ep5Ue50G5aZ1qo4GbL8aK8Fgp1FMRm4E6zWIp6NDom7+BY3nDZF5TG09d2w8ZzChlH1E1giLacv1riG6WvbS6MWpV4QvQ/B9Pp8ySyeEDCWOcAN7sjcQu7TNj+CfHmYGNFlpMuye8s3CiWz6ocaZ8aj7kiqpN+jORQzgLyQKfd+C6Ai/3mYaDJpF3mBsYrqjsoarX/q8yZnPfTxia/RSnnZZzTTb/JPiOivoPWwWiC9X0IIXwXFpgxy8yWk83xVIwAGPuPG2i1+Z4vyv0k/p9KvHX+dUs4=
```
