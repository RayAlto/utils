#ifndef RA_UTILS_INCLUDE_STRTOOL_HPP_
#define RA_UTILS_INCLUDE_STRTOOL_HPP_

#include <string>
#include <vector>
#include <unordered_set>
#include <functional>

namespace rayalto {
namespace utils {
namespace string {

/**
 * Trim out space characters from string (from start)
 */
std::string lstrip(std::string& str);

/**
 * Trim out space characters from string (from end)
 */
std::string rstrip(std::string& str);

/**
 * Trim out space characters from string (from both ends)
 */
std::string strip(std::string& str);

/**
 * Trim out specified characters from string (from start)
 */
std::string lstrip(std::string& str, const std::unordered_set<char>& chars);

/**
 * Trim out specified characters from string (from end)
 */
std::string rstrip(std::string& str, const std::unordered_set<char>& chars);

/**
 * Trim out specified characters from string (from both ends)
 */
std::string strip(std::string& str, const std::unordered_set<char>& chars);

/**
 * Split string with specified char
 */
std::vector<std::string> split(const std::string& str, const char& sep);

/**
 * Split string with specified string
 */
std::vector<std::string> split(const std::string& str, const std::string& sep);

/**
 * Split string with specified char and with callback to determine whether to leave a line
 */
std::vector<std::string> split(
    const std::string& str,
    const char& sep,
    std::function<bool(std::string& line)>& callback);

/**
 * Split string with specified string and with callback to determine whether to leave a line
 */
std::vector<std::string> split(
    const std::string& str,
    const std::string& sep,
    std::function<bool(std::string& line)>& callback);

/**
 * Split string with specified char and with callback to determine whether to leave a line
 */
std::vector<std::string> split(const std::string& str,
                               const char& sep,
                               bool (*callback)(std::string& line));

/**
 * Split string with specified string and with callback to determine whether to leave a line
 */
std::vector<std::string> split(const std::string& str,
                               const std::string& sep,
                               bool (*callback)(std::string& line));

/**
 * Split string with specified char only once
 */
std::pair<std::string, std::string> split_once(const std::string& str,
                                               const char& sep);

/**
 * Split string with specified string only once
 */
std::pair<std::string, std::string> split_once(const std::string& str,
                                               const std::string& sep);

} // namespace string
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_INCLUDE_STRTOOL_HPP_
