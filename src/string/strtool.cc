#include "string/strtool.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <functional>
#include <iterator>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace rayalto {
namespace utils {
namespace string {

std::string lstrip(std::string&& str) {
    return lstrip(str);
}

std::string lstrip(std::string& str) {
    str.erase(
        str.begin(),
        std::find_if(str.begin(), str.end(), [](unsigned char ch) -> bool {
            return !std::isspace(ch);
        }));
    return str;
}

std::string lstrip(const std::string& str) {
    std::string result(str);
    result.erase(
        result.begin(),
        std::find_if(
            result.begin(), result.end(), [](unsigned char ch) -> bool {
                return !std::isspace(ch);
            }));
    return result;
}

std::string rstrip(std::string&& str) {
    return rstrip(str);
}

std::string rstrip(std::string& str) {
    str.erase(std::find_if(str.rbegin(),
                           str.rend(),
                           [](unsigned char ch) { return !std::isspace(ch); })
                  .base(),
              str.end());
    return str;
}

std::string rstrip(const std::string& str) {
    std::string result(str);
    result.erase(
        std::find_if(result.rbegin(),
                     result.rend(),
                     [](unsigned char ch) { return !std::isspace(ch); })
            .base(),
        result.end());
    return result;
}

std::string strip(std::string&& str) {
    return strip(str);
}

std::string strip(std::string& str) {
    lstrip(str);
    return rstrip(str);
}

std::string strip(const std::string& str) {
    std::string result(str);
    lstrip(result);
    rstrip(result);
    return result;
}

std::string lstrip(std::string&& str, const std::unordered_set<char>& chars) {
    return lstrip(str, chars);
}

std::string lstrip(std::string& str, const std::unordered_set<char>& chars) {
    if (chars.empty()) {
        return lstrip(str);
    }
    str.erase(str.begin(),
              std::find_if(
                  str.begin(), str.end(), [&chars](unsigned char ch) -> bool {
                      return (chars.count(ch) == 0);
                  }));
    return str;
}

std::string lstrip(const std::string& str,
                   const std::unordered_set<char>& chars) {
    std::string result(str);
    if (chars.empty()) {
        return lstrip(result);
    }
    result.erase(
        result.begin(),
        std::find_if(
            result.begin(), result.end(), [&chars](unsigned char ch) -> bool {
                return (chars.count(ch) == 0);
            }));
    return result;
}

std::string rstrip(std::string&& str, const std::unordered_set<char>& chars) {
    return rstrip(str, chars);
}

std::string rstrip(std::string& str, const std::unordered_set<char>& chars) {
    if (chars.empty()) {
        return rstrip(str);
    }
    str.erase(std::find_if(
                  str.rbegin(),
                  str.rend(),
                  [&chars](unsigned char ch) { return (chars.count(ch) == 0); })
                  .base(),
              str.end());
    return str;
}

std::string rstrip(const std::string& str,
                   const std::unordered_set<char>& chars) {
    std::string result(str);
    if (chars.empty()) {
        return rstrip(result);
    }
    result.erase(std::find_if(result.rbegin(),
                              result.rend(),
                              [&chars](unsigned char ch) {
                                  return (chars.count(ch) == 0);
                              })
                     .base(),
                 result.end());
    return result;
}

std::string strip(std::string&& str, const std::unordered_set<char>& chars) {
    return strip(str, chars);
}

std::string strip(std::string& str, const std::unordered_set<char>& chars) {
    lstrip(str, chars);
    return rstrip(str, chars);
}

std::string strip(const std::string& str,
                  const std::unordered_set<char>& chars) {
    std::string result(str);
    lstrip(result, chars);
    rstrip(result, chars);
    return result;
}

std::vector<std::string> split(const std::string& str, const char& sep) {
    std::istringstream str_stream(str);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(str_stream, line, sep)) {
        lines.emplace_back(line);
    }
    return lines;
}

std::vector<std::string> split(const std::string& str, const std::string& sep) {
    std::size_t line_start = 0;
    std::size_t line_end = 0;
    std::size_t sep_size = sep.size();
    std::vector<std::string> lines;
    do {
        line_end = str.find(sep, line_start);
        lines.emplace_back(str.substr(line_start, line_end - line_start));
        line_start = line_end + sep_size;
    } while (line_end != std::string::npos);
    return lines;
}

std::vector<std::string> split(const std::string& str,
                               const char& sep,
                               std::function<bool(std::string&)>& callback) {
    std::istringstream str_stream(str);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(str_stream, line, sep)) {
        if (callback(line)) {
            lines.emplace_back(line);
        }
    }
    return lines;
}

std::vector<std::string> split(const std::string& str,
                               const std::string& sep,
                               std::function<bool(std::string&)>& callback) {
    std::size_t line_start = 0;
    std::size_t line_end = 0;
    std::size_t sep_size = sep.size();
    std::vector<std::string> lines;
    std::string line;
    do {
        line_end = str.find(sep, line_start);
        line = str.substr(line_start, line_end - line_start);
        if (callback(line)) {
            lines.emplace_back(line);
        }
        line_start = line_end + sep_size;
    } while (line_end != std::string::npos);
    return lines;
}

std::vector<std::string> split(const std::string& str,
                               const char& sep,
                               bool (*callback)(std::string&)) {
    std::istringstream str_stream(str);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(str_stream, line, sep)) {
        if (callback(line)) {
            lines.emplace_back(line);
        }
    }
    return lines;
}

std::vector<std::string> split(const std::string& str,
                               const std::string& sep,
                               bool (*callback)(std::string&)) {
    std::size_t line_start = 0;
    std::size_t line_end = 0;
    std::size_t sep_size = sep.size();
    std::vector<std::string> lines;
    std::string line;
    do {
        line_end = str.find(sep, line_start);
        line = str.substr(line_start, line_end - line_start);
        if (callback(line)) {
            lines.emplace_back(line);
        }
        line_start = line_end + sep_size;
    } while (line_end != std::string::npos);
    return lines;
}

std::pair<std::string, std::string> split_once(const std::string& str,
                                               const char& sep) {
    std::size_t sep_index = str.find(sep);
    if (sep_index == std::string::npos) {
        return std::make_pair(str, std::string {});
    }
    return std::make_pair(str.substr(0, sep_index), str.substr(sep_index + 1));
}

std::pair<std::string, std::string> split_once(const std::string& str,
                                               const std::string& sep) {
    std::size_t sep_index = str.find(sep);
    if (sep_index == std::string::npos) {
        return std::make_pair(str, std::string {});
    }
    return std::make_pair(str.substr(0, sep_index),
                          str.substr(sep_index + sep.size()));
}

std::string kv_format(const std::map<std::string, std::string>& kvs,
                      const char& kv_delimiter,
                      const bool& kv_space,
                      const char& item_delimiter,
                      const bool& item_sapce,
                      const bool& item_delimiter_end) {
    std::size_t kv_count = kvs.size();
    std::size_t kv_index_end = kv_count - 1;
    std::size_t kv_index = 0;
    std::map<std::string, std::string>::const_iterator kv = kvs.begin();
    std::string result;
    result.reserve(kv_count * 8);
    while (kv != kvs.end()) {
        result.append(kv->first);
        result.push_back(kv_delimiter);
        if (kv_space) {
            result.push_back(' ');
        }
        result.append(kv->second);
        if (item_delimiter_end || (kv_index != kv_index_end)) {
            result.push_back(item_delimiter);
            if (item_sapce) {
                result.push_back(' ');
            }
        }
        kv++;
        kv_index++;
    }
    return result;
}

bool compare_ic(const std::string& lv, const std::string& rv) {
    return std::equal(lv.begin(),
                      lv.end(),
                      rv.begin(),
                      [](unsigned char lc, unsigned char rc) {
                          return std::tolower(lc) == std::tolower(rc);
                      });
}

std::string join(const std::string& str,
                 const std::initializer_list<std::string>& between) {
    return std::accumulate(std::next(between.begin()),
                           between.end(),
                           *between.begin(),
                           [&str](std::string l, std::string r) -> std::string {
                               return std::move(l) + std::move(str)
                                      + std::move(r);
                           });
}

std::string join(std::string&& str,
                 std::initializer_list<std::string>&& between) {
    return std::accumulate(std::next(between.begin()),
                           between.end(),
                           *between.begin(),
                           [&str](std::string l, std::string r) -> std::string {
                               return std::move(l) + std::move(str)
                                      + std::move(r);
                           });
}

std::string random_string(const std::size_t& len) {
    return random_string(
        len, {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C',
              'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
              'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c',
              'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
              'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'});
}

std::string random_string(const std::size_t& len,
                          const std::vector<char>& characters) {
    std::string foo(len, 0);
    char result[len + 1];
    std::random_device r_device;
    std::mt19937 random_engine(r_device());
    std::uniform_int_distribution<std::size_t> distribution(
        0, characters.size() - 1);
    for (int i = 0; i < len; ++i) {
        result[i] = characters[distribution(random_engine)];
    }
    result[len] = '\0';
    return result;
}

} // namespace string
} // namespace utils
} // namespace rayalto
