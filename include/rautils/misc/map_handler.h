#ifndef RA_UTILS_UTIL_MAP_HANDLER_H_
#define RA_UTILS_UTIL_MAP_HANDLER_H_

#include <algorithm> // std::lexicographical_compare
#include <cctype> // std::tolower
#include <cstddef>
#include <initializer_list>
#include <map>
#include <string>
#include <utility>

namespace rayalto {
namespace utils {
namespace misc {

/**
 * Ignore case version of std::less
 */
struct LessIC {
    struct LessCompareIC {
        bool operator()(const unsigned char& lc,
                        const unsigned char& rc) const noexcept {
            return std::tolower(lc) < std::tolower(rc);
        }
    };
    bool operator()(const std::string& lv,
                    const std::string& rv) const noexcept {
        return std::lexicographical_compare(
            lv.begin(), lv.end(), rv.begin(), rv.end(), LessCompareIC());
    }
};

// imitating dict in python
using Dict = std::map<std::string, std::string>;
// imitating dict in python, case insensitive version
using DictIC = std::map<std::string, std::string, LessIC>;

/**
 * Stupid wrap of std::map<std::string, ValueType> with virtual destructor
 * and some extra stupid functions, case insensitive version
 */
template <typename ValueType>
class MapIc {
    using MapType = std::map<std::string, ValueType, LessIC>;
    using iterator = typename MapType::iterator;
    using const_iterator = typename MapType::const_iterator;

public:
    MapIc(const MapType& map) : map_(map) {}
    MapIc(MapType&& map) : map_(std::move(map)) {}
    MapIc(
        std::initializer_list<std::pair<const std::string, ValueType>> pairs) :
        map_(pairs) {}

    MapIc() = default;
    MapIc(const MapIc&) = default;
    MapIc(MapIc&&) noexcept = default;

    MapIc& operator=(const MapIc&) = default;
    MapIc& operator=(MapIc&&) noexcept = default;

    virtual ~MapIc() = default;

    std::string& operator[](const std::string& name) {
        return map_[name];
    }

    iterator begin() {
        return map_.begin();
    }
    const_iterator begin() const {
        return map_.begin();
    }

    const_iterator cbegin() const {
        return map_.cbegin();
    }

    iterator end() {
        return map_.end();
    }
    const_iterator end() const {
        return map_.end();
    }

    const_iterator cend() const {
        return map_.cend();
    }

    void clear() {
        return map_.clear();
    }

    bool empty() const noexcept {
        return map_.empty();
    }

    std::size_t size() const noexcept {
        return map_.size();
    }

    iterator find(const std::string& key) {
        return map_.find(key);
    }
    const_iterator find(const std::string& key) const {
        return map_.find(key);
    }

    iterator erase(iterator pos) {
        return map_.erase(pos);
    }
    iterator erase(const_iterator pos) {
        return map_.erase(pos);
    }
    iterator erase(const_iterator first, const_iterator last) {
        return map_.erase(first, last);
    }

    std::pair<iterator, bool> insert(
        const std::pair<std::string, ValueType>& item) {
        return map_.insert(item);
    }
    std::pair<iterator, bool> insert(std::pair<std::string, ValueType>&& item) {
        return map_.insert(std::move(item));
    }
    iterator insert(const_iterator hint,
                    const std::pair<std::string, ValueType>& item) {
        return map_.insert(hint, item);
    }
    iterator insert(const_iterator hint,
                    std::pair<std::string, ValueType>&& item) {
        return map_.insert(hint, std::move(item));
    }

    template <class InputIt>
    void insert(InputIt first, InputIt last) {
        return map_.insert(first, last);
    }
    void insert(
        std::initializer_list<std::pair<const std::string, ValueType>> items) {
        return map_.insert(items);
    }

    // test whether a key exists
    bool exists(const std::string& key) const {
        return (map_.find(key) == map_.end());
    }

    // try to remove an item by key, return false if the key does not exists
    bool remove(const std::string& key) {
        const_iterator found = map_.find(key);
        if (found == map_.end()) {
            return false;
        }
        map_.erase(found);
        return true;
    }

    // try to remove items by key, return the amount of items actually removed
    std::size_t remove(std::initializer_list<std::string> keys) {
        std::size_t count = 0;
        for (const std::string key : keys) {
            if (remove(key)) {
                count += 1;
            }
        }
        return count;
    }

    // try to add an item, return false if the key already exists
    bool add(const std::pair<std::string, ValueType>& item) {
        return map_.insert(item).second;
    }

    // try to add an item, return false if the key already exists
    bool add(std::pair<std::string, ValueType>&& item) {
        return map_.insert(std::move(item)).second;
    }

    // try to add items, return the amount of items actually added
    std::size_t add(
        std::initializer_list<std::pair<std::string, ValueType>> items) {
        std::size_t count = 0;
        for (std::pair<std::string, std::string> item : items) {
            if (add(item)) {
                count += 1;
            }
        }
        return count;
    }

    // try to update/add an item, return false if a same item is already exists
    bool update(const std::pair<std::string, ValueType>& item) {
        std::string& previous_value = map_[item.first];
        if (previous_value == item.second) {
            return false;
        }
        previous_value = item.second;
        return true;
    }

    // try to update/add an item, return false if a same item is already exists
    bool update(std::pair<std::string, ValueType>&& item) {
        std::string& previous_value = map_[item.first];
        if (previous_value == item.second) {
            return false;
        }
        previous_value = std::move(item.second);
        return true;
    }

    // try to update/add items, return the amount of items actually updated/added
    std::size_t update(
        std::initializer_list<std::pair<std::string, ValueType>> items) {
        std::size_t count = 0;
        for (std::pair<std::string, std::string> item : items) {
            if (update(item)) {
                count += 1;
            }
        }
        return count;
    }

protected:
    MapType map_;
};

/**
 * Stupid wrap of std::map<std::string, ValueType> with virtual destructor
 * and some extra stupid functions
 */
template <typename ValueType>
class Map {
    using MapType = std::map<std::string, ValueType>;
    using iterator = typename MapType::iterator;
    using const_iterator = typename MapType::const_iterator;

public:
    Map(const MapType& map) : map_(map) {}
    Map(MapType&& map) : map_(std::move(map)) {}
    Map(std::initializer_list<std::pair<const std::string, ValueType>> pairs) :
        map_(pairs) {}

    Map() = default;
    Map(const Map&) = default;
    Map(Map&&) noexcept = default;

    Map& operator=(const Map&) = default;
    Map& operator=(Map&&) noexcept = default;

    virtual ~Map() = default;

    std::string& operator[](const std::string& name) {
        return map_[name];
    }

    iterator begin() {
        return map_.begin();
    }
    const_iterator begin() const {
        return map_.begin();
    }

    const_iterator cbegin() const {
        return map_.cbegin();
    }

    iterator end() {
        return map_.end();
    }
    const_iterator end() const {
        return map_.end();
    }

    const_iterator cend() const {
        return map_.cend();
    }

    void clear() {
        return map_.clear();
    }

    bool empty() const noexcept {
        return map_.empty();
    }

    std::size_t size() const noexcept {
        return map_.size();
    }

    iterator find(const std::string& key) {
        return map_.find(key);
    }
    const_iterator find(const std::string& key) const {
        return map_.find(key);
    }

    iterator erase(iterator pos) {
        return map_.erase(pos);
    }
    iterator erase(const_iterator pos) {
        return map_.erase(pos);
    }
    iterator erase(const_iterator first, const_iterator last) {
        return map_.erase(first, last);
    }

    std::pair<iterator, bool> insert(
        const std::pair<std::string, ValueType>& item) {
        return map_.insert(item);
    }
    std::pair<iterator, bool> insert(std::pair<std::string, ValueType>&& item) {
        return map_.insert(std::move(item));
    }
    iterator insert(const_iterator hint,
                    const std::pair<std::string, ValueType>& item) {
        return map_.insert(hint, item);
    }
    iterator insert(const_iterator hint,
                    std::pair<std::string, ValueType>&& item) {
        return map_.insert(hint, std::move(item));
    }

    template <class InputIt>
    void insert(InputIt first, InputIt last) {
        return map_.insert(first, last);
    }
    void insert(
        std::initializer_list<std::pair<const std::string, ValueType>> items) {
        return map_.insert(items);
    }

    // test whether a key exists
    bool exists(const std::string& key) const {
        return (map_.find(key) == map_.end());
    }

    // try to remove an item by key, return false if the key does not exists
    bool remove(const std::string& key) {
        const_iterator found = map_.find(key);
        if (found == map_.end()) {
            return false;
        }
        map_.erase(found);
        return true;
    }

    // try to remove items by key, return the amount of items actually removed
    std::size_t remove(std::initializer_list<std::string> keys) {
        std::size_t count = 0;
        for (const std::string key : keys) {
            if (remove(key)) {
                count += 1;
            }
        }
        return count;
    }

    // try to add an item, return false if the key already exists
    bool add(const std::pair<std::string, ValueType>& item) {
        return map_.insert(item).second;
    }

    // try to add an item, return false if the key already exists
    bool add(std::pair<std::string, ValueType>&& item) {
        return map_.insert(std::move(item)).second;
    }

    // try to add items, return the amount of items actually added
    std::size_t add(
        std::initializer_list<std::pair<std::string, ValueType>> items) {
        std::size_t count = 0;
        for (std::pair<std::string, ValueType> item : items) {
            if (add(item)) {
                count += 1;
            }
        }
        return count;
    }

    // try to update/add an item, return false if a same item is already exists
    bool update(const std::pair<std::string, ValueType>& item) {
        std::string& previous_value = map_[item.first];
        if (previous_value == item.second) {
            return false;
        }
        previous_value = item.second;
        return true;
    }

    // try to update/add an item, return false if a same item is already exists
    bool update(std::pair<std::string, ValueType>&& item) {
        std::string& previous_value = map_[item.first];
        if (previous_value == item.second) {
            return false;
        }
        previous_value = std::move(item.second);
        return true;
    }

    // try to update/add items, return the amount of items actually updated/added
    std::size_t update(
        std::initializer_list<std::pair<std::string, ValueType>> items) {
        std::size_t count = 0;
        for (std::pair<std::string, std::string> item : items) {
            if (update(item)) {
                count += 1;
            }
        }
        return count;
    }

protected:
    MapType map_;
};

using DictHandler = Map<std::string>;
using DictIcHandler = MapIc<std::string>;

} // namespace misc
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_UTIL_MAP_HANDLER_H_
