#ifndef RA_UTILS_STRING_STRTOOL_HPP_
#define RA_UTILS_STRING_STRTOOL_HPP_

#include <cstddef>
#include <functional>
#include <map>
#include <numeric>
#include <string>
#include <unordered_set>
#include <vector>

namespace rayalto {
namespace utils {
namespace string {

namespace __private__ {

template <typename Iter>
std::string join(std::string&& str, Iter begin, Iter end) {
    return std::accumulate(std::next(begin),
                           end,
                           *begin,
                           [&str](std::string l, std::string r) -> std::string {
                               return std::move(l) + std::move(str)
                                      + std::move(r);
                           });
}

} // namespace __private__

// trim out space characters from string (from start)
std::string lstrip(std::string&& str);
std::string lstrip(std::string& str);
std::string lstrip(const std::string& str);

// trim out space characters from string (from end)
std::string rstrip(std::string&& str);
std::string rstrip(std::string& str);
std::string rstrip(const std::string& str);

// trim out space characters from string (from both ends)
std::string strip(std::string&& str);
std::string strip(std::string& str);
std::string strip(const std::string& str);

// trim out specified characters from string (from start)
std::string lstrip(std::string&& str, const std::unordered_set<char>& chars);
std::string lstrip(std::string& str, const std::unordered_set<char>& chars);
std::string lstrip(const std::string& str,
                   const std::unordered_set<char>& chars);

// trim out specified characters from string (from end)
std::string rstrip(std::string&& str, const std::unordered_set<char>& chars);
std::string rstrip(std::string& str, const std::unordered_set<char>& chars);
std::string rstrip(const std::string& str,
                   const std::unordered_set<char>& chars);

// trim out specified characters from string (from both ends)
std::string strip(std::string&& str, const std::unordered_set<char>& chars);
std::string strip(std::string& str, const std::unordered_set<char>& chars);
std::string strip(const std::string& str,
                  const std::unordered_set<char>& chars);

// split string with specified char
std::vector<std::string> split(const std::string& str, const char& sep);

// split string with specified string
std::vector<std::string> split(const std::string& str, const std::string& sep);

// split string with specified char and with callback to determine whether to leave a line
std::vector<std::string> split(
    const std::string& str,
    const char& sep,
    std::function<bool(std::string& line)>& callback);

// split string with specified string and with callback to determine whether to leave a line
std::vector<std::string> split(
    const std::string& str,
    const std::string& sep,
    std::function<bool(std::string& line)>& callback);

// split string with specified char and with callback to determine whether to leave a line
std::vector<std::string> split(const std::string& str,
                               const char& sep,
                               bool (*callback)(std::string& line));

// split string with specified string and with callback to determine whether to leave a line
std::vector<std::string> split(const std::string& str,
                               const std::string& sep,
                               bool (*callback)(std::string& line));

// split string with specified char only once
std::pair<std::string, std::string> split_once(const std::string& str,
                                               const char& sep);

// split string with specified string only once
std::pair<std::string, std::string> split_once(const std::string& str,
                                               const std::string& sep);

// format a map to std::string like: "key1: value1, key2: value2"
// clang-format off
std::string kv_format(
    const std::map<std::string, std::string>& kvs,
    const char& kv_delimiter = ':',       // "key<?>value"
    const bool& kv_space = true,          // "key: value" or "key:value"
    const char& item_delimiter = ',',     // "k1:v1<?>k2:v2"
    const bool& item_sapce = true,        // "k1:v1, k2:v2" or "k1:v1,k2:v2"
    const bool& item_delimiter_end = true // "k1:v1,k2:v2," or "k1:v1,k2:v2"
);
// clang-format on

// case insensitive string compare
bool compare_ic(const std::string& lv, const std::string& rv);

// string join
std::string join(const std::string& str,
                 const std::initializer_list<std::string>& between);
std::string join(std::string&& str,
                 std::initializer_list<std::string>&& between);
template <typename Iter>
std::string join(std::string&& str, Iter begin, Iter end) {
    return __private__::join(std::move(str), begin, end);
}

std::string random_string(const std::size_t& len = 32);
std::string random_string(const std::size_t& len,
                          const std::vector<char>& characters);

} // namespace string
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_STRING_STRTOOL_HPP_
