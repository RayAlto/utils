#include "rautils/system/subprocess.h"

#include <cstddef>
#include <cstdio>
#include <memory>
#include <string>
#include <utility>

#include "uv.h"

namespace rayalto {
namespace utils {
namespace system {

namespace {

static uv_alloc_cb uv_alloc =
    [](uv_handle_t*, std::size_t suggested_size, uv_buf_t* uv_buf) -> void {
    uv_buf->base = new char[suggested_size] {};
    uv_buf->len = suggested_size;
};

struct Result {
    bool& exited_;
    std::int64_t& exit_status_;
    int& term_signal_;
    std::string& stdout_;
    std::string& stderr_;
};

} // anonymous namespace

Subprocess::Subprocess() : args_(std::make_unique<Args>()) {}

Subprocess::Subprocess(const Subprocess& subprocess) {
    if (subprocess.args_ != nullptr) {
        args_ = std::make_unique<Args>(*subprocess.args_);
    }
}

Subprocess& Subprocess::operator=(const Subprocess& subprocess) {
    if (this == &subprocess) {
        return *this;
    }
    if (subprocess.args_ != nullptr) {
        args_ = std::make_unique<Args>(*subprocess.args_);
    }
    return *this;
}

const Subprocess::Args& Subprocess::args() const {
    return *args_;
}

Subprocess::Args& Subprocess::args() {
    return *args_;
}

Subprocess& Subprocess::args(const Args& args) {
    args_ = std::make_unique<Args>(args);
    return *this;
}

Subprocess& Subprocess::args(Args&& args) {
    args_ = std::make_unique<Args>(std::move(args));
    return *this;
}

Subprocess& Subprocess::run() {
    Args& args = *args_;
    if (args.size() == 0) {
        return *this;
    }
    uv_loop_t uv_loop {};
    uv_loop_init(&uv_loop);

    uv_process_t uv_process {};
    // for those fucking disgusting function pointers
    Result result {exited_, exit_status_, term_signal_, stdout_, stderr_};
    uv_process.data = &result;

    uv_pipe_t uv_process_pipe_stdout;
    uv_pipe_t uv_process_pipe_stderr;
    uv_pipe_init(&uv_loop, &uv_process_pipe_stdout, 0);
    uv_pipe_init(&uv_loop, &uv_process_pipe_stderr, 0);
    uv_process_pipe_stdout.data = &result;
    uv_process_pipe_stderr.data = &result;

    uv_stdio_container_t uv_process_stdio[3];
    uv_process_stdio[0].flags = UV_IGNORE;
    uv_process_stdio[1].flags =
        static_cast<uv_stdio_flags>(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
    uv_process_stdio[1].data.stream =
        reinterpret_cast<uv_stream_t*>(&uv_process_pipe_stdout);
    uv_process_stdio[2].flags =
        static_cast<uv_stdio_flags>(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
    uv_process_stdio[2].data.stream =
        reinterpret_cast<uv_stream_t*>(&uv_process_pipe_stderr);

    uv_process_options_t uv_process_options {};
    uv_process_options.exit_cb = [](uv_process_t* uv_process,
                                    std::int64_t exit_status,
                                    int term_signal) -> void {
        Result& result = *reinterpret_cast<Result*>(uv_process->data);
        result.exit_status_ = exit_status;
        result.term_signal_ = term_signal;
        result.exited_ = true;
        uv_close(reinterpret_cast<uv_handle_t*>(uv_process), nullptr);
    };
    uv_process_options.file = args[0].c_str();
    uv_process_options.args = args.c_str_array();
    uv_process_options.stdio_count = 3;
    uv_process_options.stdio = uv_process_stdio;

    int uv_return_code = uv_spawn(&uv_loop, &uv_process, &uv_process_options);
    if (uv_return_code) {
        exited_ = true;
        exit_status_ = -1;
        stderr_ = uv_err_name(uv_return_code);
        stderr_ += ": ";
        stderr_ += uv_strerror(uv_return_code);
        return *this;
    }

    uv_read_start(
        reinterpret_cast<uv_stream_t*>(&uv_process_pipe_stdout),
        uv_alloc,
        [](uv_stream_t* uv_stream,
           ssize_t nread,
           const uv_buf_t* uv_buf) -> void {
            if (nread > 0) {
                Result& result = *reinterpret_cast<Result*>(uv_stream->data);
                result.stdout_.append(uv_buf->base, uv_buf->len);
            }
            if (uv_buf->base != nullptr) {
                delete[] uv_buf->base;
            }
            if (nread == UV_EOF) {
                uv_close(reinterpret_cast<uv_handle_t*>(uv_stream), nullptr);
            }
        });
    uv_read_start(
        reinterpret_cast<uv_stream_t*>(&uv_process_pipe_stderr),
        uv_alloc,
        [](uv_stream_t* uv_stream,
           ssize_t nread,
           const uv_buf_t* uv_buf) -> void {
            if (nread > 0) {
                Result& result = *reinterpret_cast<Result*>(uv_stream->data);
                result.stderr_.append(uv_buf->base, uv_buf->len);
            }
            if (uv_buf->base != nullptr) {
                delete[] uv_buf->base;
            }
            if (nread == UV_EOF) {
                uv_close(reinterpret_cast<uv_handle_t*>(uv_stream), nullptr);
            }
        });

    uv_run(&uv_loop, UV_RUN_DEFAULT);
    uv_loop_close(&uv_loop);
    return *this;
}

const bool& Subprocess::exited() const {
    return exited_;
}

const std::int64_t& Subprocess::get_exit_status() const {
    return exit_status_;
}

const std::string& Subprocess::get_stdout() const {
    return stdout_;
}

const std::string& Subprocess::get_stderr() const {
    return stderr_;
}

} // namespace system
} // namespace utils
} // namespace rayalto
