#pragma once

#include "headers.hpp"
#include <vector>

template <typename T>
using SharedDArray = std::shared_ptr<T[]>;

template <typename T>
using DArray = std::vector<T>;

template <typename T>
using POINT = T*;