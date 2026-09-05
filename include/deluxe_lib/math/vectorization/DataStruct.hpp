#pragma once

#include "deluxe_lib/core/core.hpp"

#include <vector>
#include <concepts>

#include "deluxe_lib/math/vectorization/DataVector.hpp"

namespace concepts::math {
    template<class T>
    concept type_is_DataStruct = std::derived_from<T, DataStruct<T>>;
}

template<class STRUCT>
class DataStruct
{
public:
    DataStruct() = default;
    ~DataStruct() = default;

private:
};