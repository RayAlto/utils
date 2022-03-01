#ifndef RA_UTILS_TIMER_HPP_
#define RA_UTILS_TIMER_HPP_

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
        function_(function), interval_(interval) {
        work_thread_ = std::thread([this]() {
            std::unique_lock<std::mutex> lock(mutex_);
            while (true) {
                while (!working_) {
                    // stop() called / start() has not been called
                    done_.notify_all(); // completed, notify stop()
                    start_.wait(lock); // wait for the next start()
                }
                if (exiting_) {
                    // ~Timer() called
                    break;
                }
                lock.unlock();
                std::this_thread::sleep_for(interval_);
                function_();
                lock.lock();
            }
        });
    }
    explicit Timer(const Timer&) = delete;
    explicit Timer(Timer&&) = default;
    Timer& operator=(const Timer&) = delete;
    Timer& operator=(Timer&&) = default;

    virtual ~Timer() {
        std::unique_lock<std::mutex> lock(mutex_);
        exiting_ = true;
        working_ = true; // make work thread able to continue
        if (work_thread_.joinable()) {
            // work thread has not been started
            work_thread_.join();
        }
        // make work thread to continue
        start_.notify_all();
    }

    void set_interval(const precision& interval) {
        interval_ = interval;
    }

    template <class Rep, class Period>
    void set_interval(const std::chrono::duration<Rep, Period>& interval) {
        std::unique_lock<std::mutex> lock(mutex_);
        interval_ =
            std::chrono::duration_cast<precision, Rep, Period>(interval);
    }

    void start() {
        std::unique_lock<std::mutex> lock(mutex_);
        working_ = true;
        if (work_thread_.joinable()) {
            // work has not been started
            // start work thread in background
            work_thread_.detach();
        }
        // make work thread to continue
        start_.notify_all();
    }

    void stop() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (work_thread_.joinable()) {
            // work thread is not started yet
            return;
        }
        // block work thread from working
        working_ = false;
        // wait for work thread to complete the work at hand
        done_.wait(lock);
    }

protected:
    std::function<void(void)> function_;
    std::thread work_thread_;
    std::condition_variable done_;
    std::condition_variable start_;
    std::mutex mutex_;
    precision interval_;
    bool working_ = false;
    bool exiting_ = false;
};

} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_TIMER_HPP_
