#include "rautils/misc/uid.h"

#include <iostream>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>

#include "rautils/string/strtool.h"

namespace rayalto::utils::misc {

constexpr const char hex_map[] {"0123456789abcdef"};

constexpr int hex_value(const char& c) {
    return ((c >= '0' && c <= '9')
                ? (c - '0')
                : ((c >= 'A' && c <= 'F')
                       ? (c - 'A' + 10)
                       : ((c >= 'a' && c <= 'f') ? (c - 'a' + 10) : 0)));
}

void str_add_part(std::string& str,
                  const std::uint16_t& part,
                  const bool& add_colon,
                  const char& padding) {
    char hex_chars[4] {hex_map[(part & 0xf000) >> 12],
                       hex_map[(part & 0x0f00) >> 8],
                       hex_map[(part & 0x00f0) >> 4],
                       hex_map[(part & 0x000f)]};

    int zero_count = 0;
    while (hex_chars[zero_count] == '0') {
        ++zero_count;
    }

    if (padding != '\0') {
        for (int i = 0; i < zero_count; ++i) {
            str.push_back(padding);
        }
    }

    for (int i = zero_count; i < 4; ++i) {
        str.push_back(hex_chars[i]);
    }

    if (add_colon) {
        str.push_back(':');
    }
}

bool validate_uid_string(const std::string& uid) {
    const std::size_t uid_length = uid.length();
    if ((uid[0] == ':' && uid[1] != ':')
        || (uid[uid_length] == ':' && uid[uid_length - 1] != ':')) {
        // something like "::ffff:" or ":ffff::"
        return false;
    }
    if (string::count(uid, "::") > 1) {
        // something like "aaaa::bbbb::cccc"
        return false;
    }
    if (uid.find_first_not_of("1234567890abcdefABCDEF:") != std::string::npos) {
        // contains non-hex characters
        return false;
    }
    return true;
}

// NOLINTNEXTLINE(misc-no-recursion)
std::uint16_t parse_hex_uint16(const std::string& hex_str) {
    const std::size_t hex_str_length = hex_str.length();
    switch (hex_str_length) {
    case 0: return 0; break;
    case 1: return hex_value(hex_str[0]); break;
    case 2:
        return (hex_value(hex_str[0]) << 4) | (hex_value(hex_str[1]));
        break;
    case 3:
        return (hex_value(hex_str[0]) << 8) | (hex_value(hex_str[1]) << 4)
               | (hex_value(hex_str[2]));
        break;
    case 4:
        return (hex_value(hex_str[0]) << 12) | (hex_value(hex_str[1]) << 8)
               | (hex_value(hex_str[2]) << 4) | (hex_value(hex_str[3]));
        break;
    // invalid length, truncate
    default: return parse_hex_uint16(hex_str.substr(hex_str_length - 4)); break;
    }
}

const Uid Uid::INVALID =
    Uid(0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff);

Uid::Uid(const std::uint16_t& part1,
         const std::uint16_t& part2,
         const std::uint16_t& part3,
         const std::uint16_t& part4,
         const std::uint16_t& part5,
         const std::uint16_t& part6,
         const std::uint16_t& part7,
         const std::uint16_t& part8) :
    parts_({part1, part2, part3, part4, part5, part6, part7, part8}) {}

Uid::Uid(const std::array<std::uint16_t, 8>& parts) : parts_(parts) {}

Uid::Uid(const std::string& uid) {
    if (!validate_uid_string(uid)) {
        // invalid
        parts_.fill(0xffff);
        return;
    }
    std::vector<std::string> parts_str = string::split(uid, ':');
    std::size_t cursor = 0;
    bool zero_occurred = false; // in case of "::ffff" or "ffff::"
    for (const std::string& part_str : parts_str) {
        if (part_str.empty() && !zero_occurred) {
            zero_occurred = true;
            // met "::", move cursor of parts to right side
            cursor += 8 - parts_str.size();
        }
        parts_[cursor] = parse_hex_uint16(part_str);
        ++cursor;
    }
}

Uid& Uid::operator++() {
    ++parts_[7];
    check_carry(7);
    return *this;
}

Uid Uid::operator++(int) {
    Uid old = *this;
    operator++();
    return old;
}

Uid& Uid::operator--() {
    --parts_[7];
    check_borrow(7);
    return *this;
}

Uid Uid::operator--(int) {
    Uid old = *this;
    operator--();
    return old;
}

Uid& Uid::operator+=(const Uid& rv) {
    std::uint16_t new_value = 0;
    for (int i = 7; i > 0; --i) {
        new_value = parts_[i] + rv.parts_[i];
        if (new_value < parts_[i] && new_value < rv.parts_[i]) {
            ++parts_[i - 1];
            check_carry(i - 1);
        }
        parts_[i] = new_value;
    }
    parts_[0] += rv.parts_[0];
    return *this;
}

Uid& Uid::operator+=(const std::uint16_t& rv) {
    const std::uint16_t part8 = parts_[7];
    const std::uint16_t new_part8 = rv + parts_[7];
    if (new_part8 < part8 && new_part8 < rv) {
        ++parts_[6];
        check_carry(6);
    }
    parts_[7] = new_part8;
    return *this;
}

Uid& Uid::operator-=(const Uid& rv) {
    for (int i = 7; i > 0; --i) {
        std::uint16_t& left_part = parts_[i];
        const std::uint16_t& right_part = rv.parts_[i];
        if (left_part < right_part) {
            --parts_[i - 1];
            check_borrow(i - 1);
            left_part = 0xffff - right_part + left_part + 1;
        }
        else {
            left_part -= right_part;
        }
    }
    parts_[0] -= rv.parts_[0];
    return *this;
}

Uid& Uid::operator-=(const std::uint16_t& rv) {
    std::uint16_t& left_part = parts_[7];
    if (left_part < rv) {
        --parts_[6];
        check_borrow(6);
        left_part = 0xffff - rv + left_part + 1;
    }
    left_part -= rv;
    return *this;
}

Uid operator+(Uid lv, const Uid& rv) {
    return lv += rv;
}

Uid operator+(Uid lv, const std::uint16_t& rv) {
    return lv += rv;
}

Uid operator-(Uid lv, const Uid& rv) {
    return lv -= rv;
}

Uid operator-(Uid lv, const std::uint16_t& rv) {
    return lv -= rv;
}

bool operator<(const Uid& lv, const Uid& rv) {
    for (int i = 0; i <= 7; ++i) {
        const std::uint16_t& left_part = lv.parts_[i];
        const std::uint16_t& right_part = rv.parts_[i];
        if (left_part > right_part) {
            return false;
        }
        if (left_part < right_part) {
            return true;
        }
    }
    // all parts are equal
    return false;
}

bool operator>(const Uid& lv, const Uid& rv) {
    for (int i = 0; i <= 7; ++i) {
        const std::uint16_t& left_part = lv.parts_[i];
        const std::uint16_t& right_part = rv.parts_[i];
        if (left_part > right_part) {
            return true;
        }
        if (left_part < right_part) {
            return false;
        }
    }
    // all parts are equal
    return false;
}

bool operator<=(const Uid& lv, const Uid& rv) {
    for (int i = 0; i <= 7; ++i) {
        const std::uint16_t& left_part = lv.parts_[i];
        const std::uint16_t& right_part = rv.parts_[i];
        if (left_part > right_part) {
            return false;
        }
        if (left_part < right_part) {
            return true;
        }
    }
    // all parts are equal
    return true;
}

bool operator>=(const Uid& lv, const Uid& rv) {
    for (int i = 0; i <= 7; ++i) {
        const std::uint16_t& left_part = lv.parts_[i];
        const std::uint16_t& right_part = rv.parts_[i];
        if (left_part > right_part) {
            return true;
        }
        if (left_part < right_part) {
            return false;
        }
    }
    // all parts are equal
    return true;
}

bool operator==(const Uid& lv, const Uid& rv) {
    for (int i = 0; i <= 7; ++i) {
        if (lv.parts_[i] != rv.parts_[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const Uid& lv, const Uid& rv) {
    for (int i = 0; i <= 7; ++i) {
        if (lv.parts_[i] != rv.parts_[i]) {
            return true;
        }
    }
    return false;
}

const std::uint16_t& Uid::operator[](std::size_t index) const {
    return parts_.at(index);
}

std::uint16_t& Uid::operator[](std::size_t index) {
    return parts_.at(index);
}

std::string Uid::to_string(const bool& simplify, const char& padding) const {
    std::string result;
    result.reserve(40);
    bool simplified = false;
    for (int i = 0; i <= 7; ++i) {
        const std::uint16_t& part = parts_[i];
        if (simplify && part == 0x0000 && !simplified) {
            result.push_back(':');
            if (i == 0) {
                result.push_back(':');
            }
            while (parts_[i + 1] == 0x0000) {
                ++i;
            }
            simplified = true;
        }
        else {
            str_add_part(result, part, (i != 7), padding);
        }
    }
    return result;
}

inline void Uid::check_carry(std::size_t cursor) {
    if (parts_[cursor] != 0x0000) {
        return;
    }
    --cursor;
    while (parts_[cursor] == 0xffff && cursor > 0) {
        ++parts_[cursor];
        --cursor;
    }
    ++parts_[cursor];
}

inline void Uid::check_borrow(std::size_t cursor) {
    if (parts_[cursor] != 0xffff) {
        return;
    }
    --cursor;
    while (parts_[cursor] == 0x0000 && cursor > 0) {
        --parts_[cursor];
        --cursor;
    }
    --parts_[cursor];
}

} // namespace rayalto::utils::misc
