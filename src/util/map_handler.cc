#include "util/map_handler.h"

#include <cstddef>
#include <initializer_list>
#include <string>
#include <utility>

namespace rayalto {
namespace utils {
namespace util {

MapHandler::MapHandler(const Map& map) : map_(map) {}

MapHandler::MapHandler(Map&& map) : map_(map) {}

MapHandler::MapHandler(
    std::initializer_list<std::pair<const std::string, std::string>> pairs) :
    map_(pairs) {}

std::string& MapHandler::operator[](const std::string& name) {
    return map_[name];
}

MapHandler::iterator MapHandler::begin() {
    return map_.begin();
}

MapHandler::const_iterator MapHandler::begin() const {
    return map_.begin();
}

MapHandler::const_iterator MapHandler::cbegin() const {
    return map_.cbegin();
}

MapHandler::iterator MapHandler::end() {
    return map_.end();
}

MapHandler::const_iterator MapHandler::end() const {
    return map_.end();
}

MapHandler::const_iterator MapHandler::cend() const {
    return map_.cend();
}

void MapHandler::clear() {
    return map_.clear();
}

bool MapHandler::empty() const noexcept {
    return map_.empty();
}

std::size_t MapHandler::size() const noexcept {
    return map_.size();
}

MapHandler::iterator MapHandler::find(const std::string& key) {
    return map_.find(key);
}

MapHandler::const_iterator MapHandler::find(const std::string& key) const {
    return map_.find(key);
}

MapHandler::iterator MapHandler::erase(iterator pos) {
    return map_.erase(pos);
}

MapHandler::iterator MapHandler::erase(const_iterator pos) {
    return map_.erase(pos);
}

MapHandler::iterator MapHandler::erase(const_iterator first,
                                       const_iterator last) {
    return map_.erase(first, last);
}

std::pair<MapHandler::iterator, bool> MapHandler::insert(
    const std::pair<std::string, std::string>& item) {
    return map_.insert(item);
}

std::pair<MapHandler::iterator, bool> MapHandler::insert(
    std::pair<std::string, std::string>&& item) {
    return map_.insert(std::move(item));
}

MapHandler::iterator MapHandler::insert(
    MapHandler::const_iterator hint,
    const std::pair<std::string, std::string>& item) {
    return map_.insert(hint, item);
}

MapHandler::iterator MapHandler::insert(
    MapHandler::const_iterator hint,
    std::pair<std::string, std::string>&& item) {
    return map_.insert(hint, std::move(item));
}

template <class InputIt>
void MapHandler::insert(InputIt first, InputIt last) {
    return map_.insert(first, last);
}

void MapHandler::insert(
    std::initializer_list<std::pair<const std::string, std::string>> items) {
    return map_.insert(items);
}

bool MapHandler::exists(const std::string& key) const {
    return (map_.find(key) == map_.end());
}

bool MapHandler::remove(const std::string& key) {
    const_iterator found = map_.find(key);
    if (found == map_.end()) {
        return false;
    }
    map_.erase(found);
    return true;
}

std::size_t MapHandler::remove(std::initializer_list<std::string> keys) {
    std::size_t count = 0;
    for (const std::string key : keys) {
        if (remove(key)) {
            count += 1;
        }
    }
    return count;
}

bool MapHandler::add(const std::pair<std::string, std::string>& item) {
    return map_.insert(item).second;
}

bool MapHandler::add(std::pair<std::string, std::string>&& item) {
    return map_.insert(std::move(item)).second;
}

std::size_t MapHandler::add(
    std::initializer_list<std::pair<std::string, std::string>> items) {
    std::size_t count = 0;
    for (std::pair<std::string, std::string> item : items) {
        if (add(item)) {
            count += 1;
        }
    }
    return count;
}

bool MapHandler::update(const std::pair<std::string, std::string>& item) {
    std::string& previous_value = map_[item.first];
    if (previous_value == item.second) {
        return false;
    }
    previous_value = item.second;
    return true;
}

bool MapHandler::update(std::pair<std::string, std::string>&& item) {
    std::string& previous_value = map_[item.first];
    if (previous_value == item.second) {
        return false;
    }
    previous_value = std::move(item.second);
    return true;
}

std::size_t MapHandler::update(
    std::initializer_list<std::pair<std::string, std::string>> items) {
    std::size_t count = 0;
    for (std::pair<std::string, std::string> item : items) {
        if (update(item)) {
            count += 1;
        }
    }
    return count;
}

MapIcHandler::MapIcHandler(const MapIc& map) : map_(map) {}

MapIcHandler::MapIcHandler(MapIc&& map) : map_(map) {}

std::string& MapIcHandler::operator[](const std::string& name) {
    return map_[name];
}

MapIcHandler::iterator MapIcHandler::begin() {
    return map_.begin();
}

MapIcHandler::const_iterator MapIcHandler::begin() const {
    return map_.begin();
}

MapIcHandler::const_iterator MapIcHandler::cbegin() const {
    return map_.cbegin();
}

MapIcHandler::iterator MapIcHandler::end() {
    return map_.end();
}

MapIcHandler::const_iterator MapIcHandler::end() const {
    return map_.end();
}

MapIcHandler::const_iterator MapIcHandler::cend() const {
    return map_.cend();
}

void MapIcHandler::clear() {
    return map_.clear();
}

bool MapIcHandler::empty() const noexcept {
    return map_.empty();
}

std::size_t MapIcHandler::size() const noexcept {
    return map_.size();
}

MapIcHandler::iterator MapIcHandler::find(const std::string& key) {
    return map_.find(key);
}

MapIcHandler::const_iterator MapIcHandler::find(const std::string& key) const {
    return map_.find(key);
}

MapIcHandler::iterator MapIcHandler::erase(iterator pos) {
    return map_.erase(pos);
}

MapIcHandler::iterator MapIcHandler::erase(const_iterator pos) {
    return map_.erase(pos);
}

MapIcHandler::iterator MapIcHandler::erase(const_iterator first,
                                           const_iterator last) {
    return map_.erase(first, last);
}

std::pair<MapIcHandler::iterator, bool> MapIcHandler::insert(
    const std::pair<std::string, std::string>& item) {
    return map_.insert(item);
}

std::pair<MapIcHandler::iterator, bool> MapIcHandler::insert(
    std::pair<std::string, std::string>&& item) {
    return map_.insert(std::move(item));
}

MapIcHandler::iterator MapIcHandler::insert(
    MapIcHandler::const_iterator hint,
    const std::pair<std::string, std::string>& item) {
    return map_.insert(hint, item);
}

MapIcHandler::iterator MapIcHandler::insert(
    MapIcHandler::const_iterator hint,
    std::pair<std::string, std::string>&& item) {
    return map_.insert(hint, std::move(item));
}

template <class InputIt>
void MapIcHandler::insert(InputIt first, InputIt last) {
    return map_.insert(first, last);
}

void MapIcHandler::insert(
    std::initializer_list<std::pair<const std::string, std::string>> items) {
    return map_.insert(items);
}

bool MapIcHandler::exists(const std::string& key) const {
    return (map_.find(key) == map_.end());
}

bool MapIcHandler::remove(const std::string& key) {
    const_iterator found = map_.find(key);
    if (found == map_.end()) {
        return false;
    }
    map_.erase(found);
    return true;
}

std::size_t MapIcHandler::remove(std::initializer_list<std::string> keys) {
    std::size_t count = 0;
    for (const std::string key : keys) {
        if (remove(key)) {
            count += 1;
        }
    }
    return count;
}

bool MapIcHandler::add(const std::pair<std::string, std::string>& item) {
    return map_.insert(item).second;
}

bool MapIcHandler::add(std::pair<std::string, std::string>&& item) {
    return map_.insert(std::move(item)).second;
}

std::size_t MapIcHandler::add(
    std::initializer_list<std::pair<std::string, std::string>> items) {
    std::size_t count = 0;
    for (std::pair<std::string, std::string> item : items) {
        if (add(item)) {
            count += 1;
        }
    }
    return count;
}

bool MapIcHandler::update(const std::pair<std::string, std::string>& item) {
    std::string& previous_value = map_[item.first];
    if (previous_value == item.second) {
        return false;
    }
    previous_value = item.second;
    return true;
}

bool MapIcHandler::update(std::pair<std::string, std::string>&& item) {
    std::string& previous_value = map_[item.first];
    if (previous_value == item.second) {
        return false;
    }
    previous_value = std::move(item.second);
    return true;
}

std::size_t MapIcHandler::update(
    std::initializer_list<std::pair<std::string, std::string>> items) {
    std::size_t count = 0;
    for (std::pair<std::string, std::string> item : items) {
        if (update(item)) {
            count += 1;
        }
    }
    return count;
}

} // namespace util
} // namespace utils
} // namespace rayalto
