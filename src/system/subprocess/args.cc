#include "rautils/system/subprocess.h"

#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

namespace rayalto::utils::system {

bool operator==(const Subprocess::Args& lhs, const Subprocess::Args& rhs) {
    return lhs.args_ == rhs.args_;
}

const std::string& Subprocess::Args::at(const std::size_t& pos) const {
    return args_.at(pos);
}

std::string& Subprocess::Args::at(const std::size_t& pos) {
    return args_.at(pos);
}

const std::string& Subprocess::Args::operator[](const std::size_t& pos) const {
    return args_[pos];
}

std::string& Subprocess::Args::operator[](const std::size_t& pos) {
    return args_[pos];
}

const std::string& Subprocess::Args::front() const {
    return args_.front();
}

std::string& Subprocess::Args::front() {
    return args_.front();
}

const std::string& Subprocess::Args::back() const {
    return args_.back();
}

std::string& Subprocess::Args::back() {
    return args_.back();
}

std::string* Subprocess::Args::data() noexcept {
    return args_.data();
}

const std::string* Subprocess::Args::data() const noexcept {
    return args_.data();
}

Subprocess::Args::iterator Subprocess::Args::begin() noexcept {
    return args_.begin();
}

Subprocess::Args::const_iterator Subprocess::Args::begin() const noexcept {
    return args_.begin();
}

Subprocess::Args::const_iterator Subprocess::Args::cbegin() const noexcept {
    return args_.cbegin();
}

Subprocess::Args::iterator Subprocess::Args::end() noexcept {
    return args_.end();
}

Subprocess::Args::const_iterator Subprocess::Args::end() const noexcept {
    return args_.end();
}

Subprocess::Args::const_iterator Subprocess::Args::cend() const noexcept {
    return args_.cend();
}

Subprocess::Args::reverse_iterator Subprocess::Args::rbegin() noexcept {
    return args_.rbegin();
}

Subprocess::Args::const_reverse_iterator Subprocess::Args::rbegin()
    const noexcept {
    return args_.rbegin();
}

Subprocess::Args::const_reverse_iterator Subprocess::Args::crbegin()
    const noexcept {
    return args_.crbegin();
}

Subprocess::Args::reverse_iterator Subprocess::Args::rend() noexcept {
    return args_.rend();
}

Subprocess::Args::const_reverse_iterator Subprocess::Args::rend()
    const noexcept {
    return args_.rend();
}

Subprocess::Args::const_reverse_iterator Subprocess::Args::crend()
    const noexcept {
    return args_.crend();
}

bool Subprocess::Args::empty() const noexcept {
    return args_.empty();
}

std::size_t Subprocess::Args::size() const noexcept {
    return args_.size();
}

std::size_t Subprocess::Args::max_size() const noexcept {
    return args_.max_size();
}

void Subprocess::Args::reserve(const std::size_t& new_cap) {
    return args_.reserve(new_cap);
}

std::size_t Subprocess::Args::capacity() const noexcept {
    return args_.capacity();
}

void Subprocess::Args::shrink_to_fit() {
    return args_.shrink_to_fit();
}

void Subprocess::Args::clear() noexcept {
    return args_.clear();
}

Subprocess::Args::iterator Subprocess::Args::insert(const_iterator pos,
                                                    const std::string& value) {
    return args_.insert(pos, value);
}

Subprocess::Args::iterator Subprocess::Args::insert(const_iterator pos,
                                                    std::string&& value) {
    return args_.insert(pos, std::move(value));
}

Subprocess::Args::iterator Subprocess::Args::insert(const_iterator pos,
                                                    const std::size_t& count,
                                                    const std::string& value) {
    return args_.insert(pos, count, value);
}

Subprocess::Args::iterator Subprocess::Args::insert(
    const_iterator pos, std::initializer_list<std::string> ilist) {
    return args_.insert(pos, ilist);
}

Subprocess::Args::iterator Subprocess::Args::erase(const_iterator pos) {
    return args_.erase(pos);
}

Subprocess::Args::iterator Subprocess::Args::erase(const_iterator first,
                                                   const_iterator last) {
    return args_.erase(first, last);
}

void Subprocess::Args::push_back(const std::string& value) {
    return args_.push_back(value);
}

void Subprocess::Args::push_back(std::string&& value) {
    return args_.push_back(std::move(value));
}

void Subprocess::Args::pop_back() {
    return args_.pop_back();
}

void Subprocess::Args::resize(const std::size_t& count) {
    return args_.resize(count);
}

void Subprocess::Args::resize(const std::size_t& count,
                              const std::string& value) {
    return args_.resize(count, value);
}

void Subprocess::Args::swap(std::vector<std::string>& other) noexcept {
    return args_.swap(other);
}

Subprocess::Args::Args(const std::vector<std::string>& args) : args_(args) {}

Subprocess::Args::~Args() {
    clear_();
}

Subprocess::Args::Args(std::vector<std::string>&& args) :
    args_(std::move(args)) {}

Subprocess::Args::Args(std::initializer_list<std::string> args) : args_(args) {}

void Subprocess::Args::clear_() {
    if (data_ == nullptr) {
        return;
    }
    const std::size_t arg_count = args_.size();
    for (std::size_t i = 0; i < arg_count; ++i) {
        delete[] data_[i];
        data_[i] = nullptr;
    }
    delete[] data_;
    data_ = nullptr;
}

void Subprocess::Args::update_() {
    if (data_ != nullptr) {
        clear_();
    }
    data_ = new char*[args_.size() + 1];
    std::size_t arg_index = 0;
    const std::size_t arg_count = args_.size();
    for (; arg_index < arg_count; ++arg_index) {
        const std::string& arg = args_[arg_index];
        const std::size_t arg_length = arg.length();
        data_[arg_index] = new char[arg_length + 1];
        arg.copy(data_[arg_index], arg_length);
        data_[arg_index][arg_length] = '\0';
    }
    data_[arg_index] = nullptr;
}

char** Subprocess::Args::c_str_array() {
    update_();
    return data_;
}

} // namespace rayalto::utils::system
