#include "strtool.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <unordered_set>
#include <functional>
#include <sstream>

#include <cctype>
#include <cstddef>

namespace rayalto {
namespace utils {
namespace string {

// clang-format off

std::string lstrip(std::string& str) {
    str.erase(
        str.begin(),
        std::find_if(str.begin(), str.end(), [](unsigned char ch) -> bool {
            return !std::isspace(ch);
        }));
    return str;
}

std::string rstrip(std::string& str) {
    str.erase(
        std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(),
        str.end());
    return str;
}

std::string strip(std::string& str) {
    lstrip(str);
    return rstrip(str);
}

std::string lstrip(std::string& str, const std::unordered_set<char>& chars) {
    if (chars.empty()) {
        return lstrip(str);
    }
    str.erase(
        str.begin(),
        std::find_if(str.begin(), str.end(), [&chars](unsigned char ch) -> bool {
            return (chars.count(ch) == 0);
        }));
    return str;
}

std::string rstrip(std::string& str, const std::unordered_set<char>& chars) {
    if (chars.empty()) {
        return rstrip(str);
    }
    str.erase(
        std::find_if(str.rbegin(), str.rend(), [&chars](unsigned char ch) {
            return (chars.count(ch) == 0);
        }).base(),
        str.end());
    return str;
}

std::string strip(std::string& str, const std::unordered_set<char>& chars) {
    lstrip(str, chars);
    return rstrip(str, chars);
}

// clang-format on

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

} // namespace string
} // namespace utils
} // namespace rayalto
