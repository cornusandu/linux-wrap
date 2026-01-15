#pragma once

#include "headers.hpp"
#include "conv.hpp"

class FHandle;

namespace fs {
    void init();

    FHandle gainHandle(SharedDArray<char> path);
    SharedDArray<char> readFile(FHandle path);
}

class FHandle {
    public:
        FHandle(const char* const path);
        inline size_t get_id() { return id; }
    private:
        size_t id;
        friend SharedDArray<char> fs::readFile(FHandle path);
};
