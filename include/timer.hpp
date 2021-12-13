#ifndef RA_UTILS_INCLUDE_TIMER_HPP_
#define RA_UTILS_INCLUDE_TIMER_HPP_

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace rayalto {
namespace utils {

template <class _Rep = std::chrono::seconds::rep,
          class _Period = std::chrono::seconds::period>
class Timer {
    using precision = std::chrono::duration<_Rep, _Period>;

public:
    Timer(std::function<void(void)>&& function,
          const precision& interval = std::chrono::seconds(1)) :
        _function(function), _interval(interval) {
        _work_thread = std::thread([this]() {
            std::unique_lock<std::mutex> lock(_mutex);
            while (true) {
                while (!_working) {
                    // stop() called / start() has not been called
                    _done.notify_all(); // completed, notify stop()
                    _start.wait(lock); // wait for the next start()
                }
                if (_exiting) {
                    // ~Timer() called
                    break;
                }
                lock.unlock();
                std::this_thread::sleep_for(_interval);
                _function();
                lock.lock();
            }
        });
    }
    explicit Timer(const Timer&) = delete;
    explicit Timer(Timer&&) = default;
    Timer& operator=(const Timer&) = delete;
    Timer& operator=(Timer&&) = default;

    virtual ~Timer() {
        std::unique_lock<std::mutex> lock(_mutex);
        _exiting = true;
        _working = true; // make work thread able to continue
        if (_work_thread.joinable()) {
            // work thread has not been started
            _work_thread.join();
        }
        // make work thread to continue
        _start.notify_all();
    }

    void set_interval(const precision& interval) {
        _interval = interval;
    }

    template <class Rep, class Period>
    void set_interval(const std::chrono::duration<Rep, Period>& interval) {
        std::unique_lock<std::mutex> lock(_mutex);
        _interval =
            std::chrono::duration_cast<precision, Rep, Period>(interval);
    }

    void start() {
        std::unique_lock<std::mutex> lock(_mutex);
        _working = true;
        if (_work_thread.joinable()) {
            // work has not been started
            // start work thread in background
            _work_thread.detach();
        }
        // make work thread to continue
        _start.notify_all();
    }

    void stop() {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_work_thread.joinable()) {
            // Work thread is not started yet
            return;
        }
        // block work thread from working
        _working = false;
        // wait for work thread to complete the work at hand
        _done.wait(lock);
    }

protected:
    std::function<void(void)> _function;
    std::thread _work_thread;
    std::condition_variable _done;
    std::condition_variable _start;
    std::mutex _mutex;
    precision _interval;
    bool _working = false;
    bool _exiting = false;
};

} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_INCLUDE_TIMER_HPP_
