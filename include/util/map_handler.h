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
namespace util {

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

// std::map<std::string, std::string>
using Map = std::map<std::string, std::string>;
// std::map<std::string, std::string, LessIc>
using MapIc = std::map<std::string, std::string, LessIC>;

/**
 * Stupid wrap of std::map<std::string, std::string> with virtual destructor
 * and some extra stupid functions
 */
class MapHandler {
public:
    MapHandler(const Map& map);
    MapHandler(Map&& map);
    MapHandler(
        std::initializer_list<std::pair<const std::string, std::string>> pairs);
    MapHandler() = default;
    MapHandler(const MapHandler&) = default;
    MapHandler(MapHandler&&) noexcept = default;
    MapHandler& operator=(const MapHandler&) = default;
    MapHandler& operator=(MapHandler&&) noexcept = default;

    virtual ~MapHandler() = default;

    std::string& operator[](const std::string& name);

    using iterator = Map::iterator;
    using const_iterator = Map::const_iterator;

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;
    void clear();
    bool empty() const noexcept;
    std::size_t size() const noexcept;
    iterator find(const std::string& key);
    const_iterator find(const std::string& key) const;
    iterator erase(iterator pos);
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    std::pair<iterator, bool> insert(
        const std::pair<std::string, std::string>& item);
    std::pair<iterator, bool> insert(
        std::pair<std::string, std::string>&& item);
    iterator insert(const_iterator hint,
                    const std::pair<std::string, std::string>& item);
    iterator insert(const_iterator hint,
                    std::pair<std::string, std::string>&& item);
    template <class InputIt>
    void insert(InputIt first, InputIt last);
    void insert(
        std::initializer_list<std::pair<const std::string, std::string>> items);

    // test whether a key exists
    bool exists(const std::string& key) const;
    // try to remove an item by key, return false if the key does not exists
    bool remove(const std::string& key);
    // try to remove items by key, return the amount of items actually removed
    std::size_t remove(std::initializer_list<std::string> keys);
    // try to add an item, return false if the key already exists
    bool add(const std::pair<std::string, std::string>& item);
    // try to add an item, return false if the key already exists
    bool add(std::pair<std::string, std::string>&& item);
    // try to add items, return the amount of items actually added
    std::size_t add(
        std::initializer_list<std::pair<std::string, std::string>> items);
    // try to update/add an item, return false if a same item is already exists
    bool update(const std::pair<std::string, std::string>& item);
    // try to update/add an item, return false if a same item is already exists
    bool update(std::pair<std::string, std::string>&& item);
    // try to update/add items, return the amount of items actually updated/added
    std::size_t update(
        std::initializer_list<std::pair<std::string, std::string>> items);

protected:
    Map map_;
};

/**
 * (Ignore case version)
 * Stupid wrap of std::map<std::string, std::string> with virtual destructor
 * and some extra stupid functions
 */
class MapIcHandler {
public:
    MapIcHandler(const MapIc& map);
    MapIcHandler(MapIc&& map);
    MapIcHandler() = default;
    MapIcHandler(const MapIcHandler&) = default;
    MapIcHandler(MapIcHandler&&) noexcept = default;
    MapIcHandler& operator=(const MapIcHandler&) = default;
    MapIcHandler& operator=(MapIcHandler&&) noexcept = default;

    virtual ~MapIcHandler() = default;

    std::string& operator[](const std::string& name);

    using iterator = MapIc::iterator;
    using const_iterator = MapIc::const_iterator;

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;
    void clear();
    bool empty() const noexcept;
    std::size_t size() const noexcept;
    iterator find(const std::string& key);
    const_iterator find(const std::string& key) const;
    iterator erase(iterator pos);
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    std::pair<iterator, bool> insert(
        const std::pair<std::string, std::string>& item);
    std::pair<iterator, bool> insert(
        std::pair<std::string, std::string>&& item);
    iterator insert(const_iterator hint,
                    const std::pair<std::string, std::string>& item);
    iterator insert(const_iterator hint,
                    std::pair<std::string, std::string>&& item);
    template <class InputIt>
    void insert(InputIt first, InputIt last);
    void insert(
        std::initializer_list<std::pair<const std::string, std::string>> items);

    // test whether a key exists
    bool exists(const std::string& key) const;
    // try to remove an item by key, return false if the key does not exists
    bool remove(const std::string& key);
    // try to remove items by key, return the amount of items actually removed
    std::size_t remove(std::initializer_list<std::string> keys);
    // try to add an item, return false if the key already exists
    bool add(const std::pair<std::string, std::string>& item);
    // try to add an item, return false if the key already exists
    bool add(std::pair<std::string, std::string>&& item);
    // try to add items, return the amount of items actually added
    std::size_t add(
        std::initializer_list<std::pair<std::string, std::string>> items);
    // try to update/add an item, return false if a same item is already exists
    bool update(const std::pair<std::string, std::string>& item);
    // try to update/add an item, return false if a same item is already exists
    bool update(std::pair<std::string, std::string>&& item);
    // try to update/add items, return the amount of items actually updated/added
    std::size_t update(
        std::initializer_list<std::pair<std::string, std::string>> items);

protected:
    MapIc map_;
};

} // namespace util
} // namespace utils
} // namespace rayalto

#endif // RA_UTILS_UTIL_MAP_HANDLER_H_
