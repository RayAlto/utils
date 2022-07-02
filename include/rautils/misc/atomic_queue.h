#ifndef RA_UTILS_RAUTILS_MISC_ATOMIC_QUEUE_H_
#define RA_UTILS_RAUTILS_MISC_ATOMIC_QUEUE_H_

#include <cstddef>
#include <mutex>
#include <queue>
#include <utility>

namespace rayalto::utils::misc {

template <typename T>
class AtomicQueue {
public:
    AtomicQueue() = default;
    AtomicQueue(const AtomicQueue&) = default;
    AtomicQueue(AtomicQueue&&) noexcept = default;
    AtomicQueue& operator=(const AtomicQueue&) = default;
    AtomicQueue& operator=(AtomicQueue&&) noexcept = default;
    explicit AtomicQueue(const std::queue<T>& queue) : queue_(queue) {}
    explicit AtomicQueue(std::queue<T>&& queue) : queue_(std::move(queue)) {}

    virtual ~AtomicQueue() = default;

    T& front() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.front();
    }

    T& back() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.back();
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    std::size_t size() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.push(value);
    }

    void push(T&& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.push(std::move(value));
    }

    template <typename... Args>
    decltype(auto) emplace(Args&&... value) {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.emplace(std::forward<Args>(value)...);
    }

    void pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.pop();
    }

    void swap(AtomicQueue& q) {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.swap(q.queue_);
    }

protected:
    std::queue<T> queue_;
    std::mutex mutex_;
};

} // namespace rayalto::utils::misc

#endif // RA_UTILS_RAUTILS_MISC_ATOMIC_QUEUE_H_
