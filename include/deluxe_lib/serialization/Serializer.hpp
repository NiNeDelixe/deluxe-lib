#ifndef SERIALIZATION_SERIALIZER_HPP
#define SERIALIZATION_SERIALIZER_HPP

#include <any>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "deluxe_lib/core/core.hpp"
#include "deluxe_lib/serialization/Codec.hpp"

class Serializer
{
public:
    class Instance
    {
    public:
        template<class T, class... Args>
        using FactoryPtr = std::unique_ptr<T>(*)(Args...);

        Instance() = default;
        ~Instance() = default;

        template<class... CODECS>
        Instance& group(CODECS&&... codecs)
        {
            (addField(std::forward<CODECS>(codecs)), ...);
            return *this;
        }

        template<class T, class... Args>
        Instance& apply(FactoryPtr<T, Args...> constructor)
        {
            factory_ = std::function<std::unique_ptr<T>()>(
                [constructor]() { return constructor(Args{}...); });
            return *this;
        }

        const std::vector<std::shared_ptr<serialization_detail::FieldBase>>&
        fields() const noexcept { return fields_; }

        const std::any& factory() const noexcept { return factory_; }

    private:
        template<class F>
        void addField(F&& field)
        {
            using Decayed = std::decay_t<F>;
            fields_.push_back(std::make_shared<Decayed>(std::forward<F>(field)));
        }

        std::vector<std::shared_ptr<serialization_detail::FieldBase>> fields_;
        std::any factory_;
    };

public:
    Serializer() = default;
    ~Serializer() = default;

    template<class T>
    static Codec<T> createCodec(std::function<void(Instance&)> func);
};

template<class T>
inline Codec<T> Serializer::createCodec(std::function<void(Instance&)> func)
{
    Instance inst;
    func(inst);

    Codec<T> codec;
    for (auto& fb : inst.fields()) 
    {
        if (auto f = std::dynamic_pointer_cast<typename Codec<T>::Field>(fb))
            codec.addField(std::move(f));
    }

    if (inst.factory().has_value()) 
    {
        if (auto* f = std::any_cast<std::function<std::unique_ptr<T>()>>(&inst.factory()))
            codec.setFactory(*f);
    }

    return codec;
}

#endif // SERIALIZATION_SERIALIZER_HPP