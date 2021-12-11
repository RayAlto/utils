# RA-Utils

RayAlto's Stupid Self-use Noob Utils

## How To

### 1. subprocess

Run a command and get its stdout, stderr and exit_code. Base on `exec()` family of functions. Example:

```c++
using rayalto::util::subprocess::Result;
using rayalto::util::Subprocess;
// ...

Result result = Subprocess::run({"ls", "-ahl", "/usr/bin"});
std::cout << result.stdout << std::endl;
```

* It's **not** a shell, it will **not** expand `~` or behave like a shell.
