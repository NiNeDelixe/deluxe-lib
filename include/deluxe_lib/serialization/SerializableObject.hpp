#ifndef SERIALIZATION_SERIALIZABLEOBJECT_HPP
#define SERIALIZATION_SERIALIZABLEOBJECT_HPP

#include "deluxe_lib/core/core.hpp"

#include <concepts>

#include "deluxe_lib/serialization/Serializer.hpp"
#include "deluxe_lib/serialization/Codec.hpp"

template<class T>
class SerializableObject
{
protected:
    virtual Codec<T> codec() = 0;

private:
    
};

#endif // SERIALIZATION_SERIALIZABLEOBJECT_HPP
