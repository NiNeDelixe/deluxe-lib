#pragma once

#include "deluxe_lib/core/core.hpp"

#include <unordered_map>
#include <vector>
#include <string>

template<class T>
class DataStruct;

template<class TYPE, uint64_t type_id>
class DataVector
{
private:
    using refs_type = std::vector<std::pair<uint64_t, TYPE*>>;

public:
    DataVector() = default;
    DataVector(const TYPE& value);
    ~DataVector() = default;

public:
    operator TYPE()
    {
        return m_data;
    }

public:
    GETTER_SETTER(refs_type, Refs, m_refs)

private:
    static refs_type m_refs;
    TYPE m_data;

    // template<class T>
    // friend DataStruct<T>;
};

template<class DATA, uint64_t type_id>
std::vector<std::pair<uint64_t, DATA*>> DataVector<DATA, type_id>::m_refs = {};

template<class TYPE, uint64_t type_id> 
inline DataVector<TYPE, type_id>::DataVector(const TYPE &value)
{
    m_data = value;
    m_refs.push_back({type_id, &m_data});
}
