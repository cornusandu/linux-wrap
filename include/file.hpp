#pragma once

#include "headers.hpp"
#include "conv.hpp"

struct FHandle;

namespace fs {
    void init();

    std::shared_ptr<FHandle> &gainHandle(const char* path);
    DArray<char> readFile(std::shared_ptr<FHandle> path);
}
