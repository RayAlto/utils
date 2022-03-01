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

