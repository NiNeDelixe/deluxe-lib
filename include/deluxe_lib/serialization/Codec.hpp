#ifndef SERIALIZATION_CODEC_HPP
#define SERIALIZATION_CODEC_HPP

#include <cstdint>
#include <functional>
#include <istream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include "deluxe_lib/core/core.hpp"

namespace serialization_detail 
{

struct FieldBase
{
    virtual ~FieldBase() = default;
    virtual const char* name() const noexcept = 0;
};

} // namespace serialization_detail

template<class OBJECT>
class Codec
{
public:
    class Field : public serialization_detail::FieldBase
    {
    public:
        virtual void write(const OBJECT& obj, std::ostream& os) const = 0;
        virtual bool read(OBJECT& obj, std::istream& is) const = 0;
        virtual bool applyDefault(OBJECT& obj) const = 0;
    };

private:
    template<class Derived, class T>
    struct Primitive : Field
    {
        using MemberPtr = T OBJECT::*;

        Derived& fieldOf(const char* name)
        { name_ = name; return self(); }

        Derived& optionalFieldOf(const char* name, const T& default_val)
        { name_ = name; optional_ = true; default_ = default_val; return self(); }

        Derived& forGetter(MemberPtr p)
        { member_ = p; return self(); }

        const char* name() const noexcept override
        { return name_ ? name_ : ""; }

        void write(const OBJECT& obj, std::ostream& os) const override
        {
            if (!member_) return;
            writeValue(obj.*member_, os);
        }

        bool read(OBJECT& obj, std::istream& is) const override
        {
            if (!member_) return false;
            T val{};
            if (!readValue(val, is)) return false;
            obj.*member_ = std::move(val);
            return true;
        }

        bool applyDefault(OBJECT& obj) const override
        {
            if (!member_)       
                return true;
            if (!optional_)     
                return false;
            obj.*member_ = default_;
            return true;
        }

    private:
        Derived& self() noexcept { return static_cast<Derived&>(*this); }

        static void writeValue(const T& v, std::ostream& os)
        {
            if constexpr (std::is_same_v<T, std::string>) 
            {
                for (char c : v) 
                {
                    switch (c) 
                    {
                        case '\\': os << "\\\\"; break;
                        case '\n': os << "\\n";  break;
                        default:   os << c;      break;
                    }
                }
            } 
            else if constexpr (std::is_same_v<T, bool>) 
            {
                os << (v ? '1' : '0');
            } 
            else if constexpr (std::is_same_v<T, char>) 
            {
                os << static_cast<int>(v);
            } 
            else 
            {
                os << v;
            }
        }

        static bool readValue(T& out, std::istream& is)
        {
            if constexpr (std::is_same_v<T, std::string>) 
            {
                out.clear();
                char c = 0;
                while (is.get(c)) 
                {
                    if (c == '\\') 
                    {
                        if (!is.get(c)) 
                            return false;
                        if (c == 'n')  
                            out.push_back('\n');
                        else if (c == '\\') 
                            out.push_back('\\');
                        else                
                            out.push_back(c);
                    } 
                    else 
                    {
                        out.push_back(c);
                    }
                }
                return true;
            } 
            else if constexpr (std::is_same_v<T, bool>) 
            {
                char c = 0;
                if (!(is >> c)) 
                    return false;
                out = (c != '0');
                return true;
            } 
            else if constexpr (std::is_same_v<T, char>) 
            {
                int v = 0;
                if (!(is >> v)) 
                    return false;
                out = static_cast<char>(v);
                return true;
            } 
            else 
            {
                return static_cast<bool>(is >> out);
            }
        }

        const char* name_ = nullptr;
        MemberPtr member_ = nullptr;
        T default_{};
        bool optional_ = false;
    };

public:
    struct BOOL  : Primitive<BOOL,   bool>        {};
    struct BYTE  : Primitive<BYTE,   char>        {};
    struct SHORT : Primitive<SHORT,  int16_t>     {};
    struct INT   : Primitive<INT,    int32_t>     {};
    struct LONG  : Primitive<LONG,   int64_t>     {};
    struct FLOAT : Primitive<FLOAT,  float>       {};
    struct DOUBLE: Primitive<DOUBLE, double>      {};
    struct STRING: Primitive<STRING, std::string> {};

public:
    Codec() = default;
    ~Codec() = default;

    void addField(std::shared_ptr<Field> field) { fields_.push_back(std::move(field)); }
    void setFactory(std::function<std::unique_ptr<OBJECT>()> f) { factory_ = std::move(f); }

    void serialize(const OBJECT& obj, std::ostream& os, const bool& is_json) const
    {
        for (const auto& f : fields_) 
        {
            os << f->name() << '=';
            f->write(obj, os);
            os << '\n';
        }
    }

    bool deserialize(OBJECT& obj, std::istream& is) const
    {
        std::vector<bool> seen(fields_.size(), false);

        std::string line;
        while (std::getline(is, line)) 
        {
            if (line.empty()) 
                continue;
            const auto eq = line.find('=');
            if (eq == std::string::npos) 
                continue;

            const std::string key = line.substr(0, eq);
            const std::string val = line.substr(eq + 1);

            for (std::size_t i = 0; i < fields_.size(); ++i) 
            {
                if (key == fields_[i]->name()) 
                {
                    std::istringstream vs(val);
                    if (!fields_[i]->read(obj, vs)) 
                        return false;
                    seen[i] = true;
                    break;
                }
            }
        }

        for (std::size_t i = 0; i < fields_.size(); ++i)
            if (!seen[i] && !fields_[i]->applyDefault(obj)) 
                return false;

        return true;
    }

    std::unique_ptr<OBJECT> create() const
    {
        if (factory_) 
            return factory_();
        if constexpr (std::is_default_constructible_v<OBJECT>)
            return std::make_unique<OBJECT>();
        else
            return nullptr;
    }

    std::unique_ptr<OBJECT> deserializeNew(std::istream& is) const
    {
        auto obj = create();
        if (!obj) 
            return nullptr;
        if (!deserialize(*obj, is)) 
            return nullptr;
        return obj;
    }

private:
    std::vector<std::shared_ptr<Field>> fields_;
    std::function<std::unique_ptr<OBJECT>()> factory_;
};

#endif // SERIALIZATION_CODEC_HPP