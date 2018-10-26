#include <memory>
#include "async.h"
#include "manager.h"


namespace async {

handle_t connect(std::size_t N) {
    Manager& instance = Manager::getInstance();
    return instance.start(N);
}

void receive(handle_t handle, const char *data, std::size_t size) {
    Manager& instance = Manager::getInstance();
    instance.work(handle, data, size);
}

void disconnect(handle_t handle) {
    Manager& instance = Manager::getInstance();
    instance.stop(handle);
}

}
