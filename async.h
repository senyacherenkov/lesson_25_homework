#pragma once
#include <cstddef>

using handle_t = size_t;

namespace async {

handle_t connect(std::size_t N);
void receive(handle_t handle, const char *data, std::size_t size);
void disconnect(handle_t handle);

}
