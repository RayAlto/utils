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
            while (_working) {
                lock.unlock();
                std::this_thread::sleep_for(_interval);
                lock.lock();
                _function();
            }
            _done.notify_all();
        });
    }
    explicit Timer(const Timer&) = delete;
    explicit Timer(Timer&&) = default;
    Timer& operator=(const Timer&) = delete;
    Timer& operator=(Timer&&) = default;

    virtual ~Timer() {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_work_thread.joinable()) {
            _working = false;
            _work_thread.join();
        }
    }

    void set_interval(const precision& interval) {
        _interval = interval;
    }

    template <class Rep, class Period>
    void set_interval(const std::chrono::duration<Rep, Period>& interval) {
        std::unique_lock<std::mutex> lock(_mutex);
        // _interval = std::chrono::duration_cast<precision>(interval);
        _interval =
            std::chrono::duration_cast<precision, Rep, Period>(interval);
    }

    void start() {
        std::unique_lock<std::mutex> lock(_mutex);
        _working = true;
        if (!_work_thread.joinable()) {
            // Work thread is already running
            return;
        }
        _work_thread.detach();
    }

    void stop() {
        std::unique_lock<std::mutex> lock(_mutex);
        _working = false;
        if (_work_thread.joinable()) {
            // Work thread is not started yet
            return;
        }
        _done.wait(lock);
    }

protected:
    std::function<void(void)> _function;
    std::thread _work_thread;
    std::condition_variable _done;
    std::mutex _mutex;
    precision _interval;
    bool _working = false;
};

} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_INCLUDE_TIMER_HPP_
