#ifndef RA_UTILS_RAUTILS_SYSTEM_SUBPROCESS_H_
#define RA_UTILS_RAUTILS_SYSTEM_SUBPROCESS_H_

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace rayalto {
namespace utils {
namespace system {

class Subprocess {
public:
    class Args;

public:
    Subprocess();
    Subprocess(const Subprocess&);
    Subprocess(Subprocess&&) noexcept = default;
    Subprocess& operator=(const Subprocess&);
    Subprocess& operator=(Subprocess&&) noexcept = default;

    virtual ~Subprocess() = default;

    const Args& args() const;
    Args& args();
    Subprocess& args(const Args& args);
    Subprocess& args(Args&& args);

    Subprocess& run();

    const bool& exited() const;
    const std::int64_t& get_exit_status() const;
    const std::string& get_stdout() const;
    const std::string& get_stderr() const;

protected:
    std::unique_ptr<Args> args_ = nullptr;
    bool exited_ = false;
    std::int64_t exit_status_ = 0;
    int term_signal_ = 0;
    std::string stdout_;
    std::string stderr_;
};

class Subprocess::Args {
public:
    using iterator = std::vector<std::string>::iterator;
    using const_iterator = std::vector<std::string>::const_iterator;
    using reverse_iterator = std::vector<std::string>::reverse_iterator;
    using const_reverse_iterator =
        std::vector<std::string>::const_reverse_iterator;

public:
    Args() = default;
    Args(const Args& process_args) = default;
    Args(Args&& process_args) noexcept = default;
    Args& operator=(const Args& process_args) = default;
    Args& operator=(Args&& process_args) noexcept = default;

    Args(const std::vector<std::string>& args);
    Args(std::vector<std::string>&& args);
    Args(std::initializer_list<std::string> args);

    virtual ~Args();

    char** c_str_array();

public: /* stuff comes from std::vector<std::string> */
    const std::string& at(const std::size_t& pos) const;
    std::string& at(const std::size_t& pos);

    const std::string& operator[](const std::size_t& pos) const;
    std::string& operator[](const std::size_t& pos);

    const std::string& front() const;
    std::string& front();

    const std::string& back() const;
    std::string& back();

    std::string* data() noexcept;
    const std::string* data() const noexcept;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;

    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;

    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crend() const noexcept;

    bool empty() const noexcept;

    std::size_t size() const noexcept;

    std::size_t max_size() const noexcept;

    void reserve(const std::size_t new_cap);

    std::size_t capacity() const noexcept;

    void shrink_to_fit();

    void clear() noexcept;

    iterator insert(const_iterator pos, const std::string& value);
    iterator insert(const_iterator pos, std::string&& value);
    iterator insert(const_iterator pos,
                    const std::size_t& count,
                    const std::string& value);
    iterator insert(const_iterator pos,
                    std::initializer_list<std::string> ilist);
    template <class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        return args_.insert(pos, first, last);
    }

    template <class... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        return args_.emplace(pos, std::forward(args...));
    }

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void push_back(const std::string& value);
    void push_back(std::string&& value);

    template <class... Args>
    std::string& emplace_back(Args&&... args) {
        return args_.emplace_back(std::forward(args...));
    }

    void pop_back();

    void resize(const std::size_t& count);
    void resize(const std::size_t& count, const std::string& value);

    void swap(std::vector<std::string>& other) noexcept;

    friend bool operator==(const Args& lhs, const Args& rhs);

protected:
    void clear_();
    void update_();

protected:
    std::vector<std::string> args_;
    char** data_ = nullptr;
};

} // namespace system
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_RAUTILS_SYSTEM_SUBPROCESS_H_
