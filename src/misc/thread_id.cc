#include "rautils/misc/thread_id.h"

#include <cstddef>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace rayalto {
namespace utils {
namespace misc {

static std::unordered_map<std::thread::id, unsigned int> thread_id_map;
static std::mutex thread_id_map_lock;

unsigned int thread_id() {
    std::thread::id thread = std::this_thread::get_id();
    thread_id_map_lock.lock();
    const std::unordered_map<std::thread::id, unsigned int>::const_iterator
        found = thread_id_map.find(thread);
    if (found == thread_id_map.end()) {
        std::size_t id = thread_id_map.size();
        thread_id_map.emplace(thread, id);
        thread_id_map_lock.unlock();
        return id;
    }
    thread_id_map_lock.unlock();
    return found->second;
}

} // namespace misc
} // namespace utils
} // namespace rayalto
