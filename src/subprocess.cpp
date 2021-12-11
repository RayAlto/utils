#include "subprocess.hpp"

#include <sys/wait.h> // waitpid
#include <unistd.h> // pipe fork close dup2 read exec

#include <array>
#include <cerrno> // errno(marco)
#include <cstddef> // std::size_t
#include <cstdio> // std::exit WIFEXITED WEXITSTATUS(marco)
#include <cstring> // std::strerror
#include <memory> // std::unique_ptr
#include <stdexcept>
#include <string>
#include <vector>

#include "exceptions.hpp" // exceptions::SyscallError

namespace rayalto {
namespace utils {

namespace {

/**
 * Convert subprocess::Args to char** so that you can pass it to exec()
 *      (using subprocess::Args = std::vector<std::string>)
 */
char** args_to_argv(const subprocess::Args& args) {
    char** _argv = new char*[args.size() + 1];
    for (std::size_t i = 0; i < args.size(); i++) {
        const std::string& arg = args[i];
        _argv[i] = new char[arg.size() + 1];
        std::strcpy(_argv[i], arg.c_str());
    }
    _argv[args.size()] = nullptr;
    return _argv;
}

/**
 * Clear the argv allocated & passed to exec() when exec() failed
 *      I don't know if it is necessary
 */
void clear_argv(char** argv) {
    if (argv == nullptr) {
        return;
    }
    // clear every arg
    for (std::size_t count = 0; argv[count] != nullptr; count++) {
        delete[] argv[count];
        argv[count] = nullptr;
    }
    // delete char** argv
    delete[] argv;
    argv = nullptr;
}

} // anonymous namespace

Subprocess::Subprocess(const subprocess::Args& command_args) :
    _args(command_args) {}

Subprocess::~Subprocess() {}

subprocess::Result Subprocess::run(bool capture_output) {
    int child_stdout[2]; // Parent read from child[0], Child write to child[1]
    int child_stderr[2]; // Parent read from child[0], Child write to child[1]
    if (pipe(child_stdout) == -1) {
        throw exceptions::SyscallError(
            "Subprocess::run - pipe for child stdout");
    }
    if (pipe(child_stderr) == -1) {
        throw exceptions::SyscallError(
            "Subprocess::run - pipe for child stderr");
    }
    pid_t pid = fork();
    if (pid == -1) {
        throw exceptions::SyscallError("Subprocess::run - fork");
    }
    if (pid == 0) {
        // Children
        close(child_stdout[0]); // Close stdin
        close(child_stderr[0]); // Close stdin
        // child_stdout[1] -> new stdout for child process
        if (dup2(child_stdout[1], STDOUT_FILENO) == -1) {
            throw exceptions::SyscallError(
                "Subprocess::run - dup2 for child stdout");
        }
        // child_stderr[1] -> new stderr for child process
        if (dup2(child_stderr[1], STDERR_FILENO) == -1) {
            throw exceptions::SyscallError(
                "Subprocess::run - dup2 for child stderr");
        }
        char** argv = args_to_argv(_args);
        if (execvp(_args[0].c_str(), argv) == -1) {
            // exec failed
            clear_argv(argv);
            throw exceptions::SyscallError("Subprocess::run - execvp");
        }
    }
    // Parent
    int return_code = 0;
    std::array<char, 128> stdout_buffer;
    std::array<char, 128> stderr_buffer;
    close(child_stdout[1]); // Close stdout
    close(child_stderr[1]); // Close stdout
    ssize_t stdout_read_count = 0;
    ssize_t stderr_read_count = 0;
    // Read stdout from pipe child_stdout
    while ((stdout_read_count = read(
                child_stdout[0], stdout_buffer.data(), stdout_buffer.size()))
           > 0) {
        _result.stdout.append(stdout_buffer.data(), stdout_read_count);
    }
    if (stdout_read_count == -1) {
        throw exceptions::SyscallError("Subprocess::run - read child stdout");
    }
    // Read stderr from pipe child_stderr
    while ((stderr_read_count = read(
                child_stderr[0], stderr_buffer.data(), stderr_buffer.size()))
           > 0) {
        _result.stderr.append(stderr_buffer.data(), stderr_read_count);
    }
    if (stderr_read_count == -1) {
        throw exceptions::SyscallError("Subprocess::run - read child stderr");
    }
    // Wait and get return_code from child process
    if (waitpid(pid, &return_code, 0) == -1) {
        throw exceptions::SyscallError("Subprocess::run - waitpid");
    }

    _result.exit_normally = WIFEXITED(return_code);
    _result.exit_code = WEXITSTATUS(return_code);

    return _result;
}

subprocess::Args Subprocess::args() const {
    return _args;
}

subprocess::Result Subprocess::result() const {
    return _result;
}

subprocess::Result Subprocess::run(const subprocess::Args& args,
                                   bool capture_output) {
    Subprocess subprocess(args);
    return subprocess.run(capture_output);
}

subprocess::Result Subprocess::run(const char* arg, bool capture_output) {
    return Subprocess::run(subprocess::Args {arg});
}

} // namespace utils
} // namespace rayalto
