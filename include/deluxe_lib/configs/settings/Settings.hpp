#ifndef SETTINGS_SETTINGS_HPP
#define SETTINGS_SETTINGS_HPP

#include "deluxe_lib/core/core.hpp"

#include <limits>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <math.h>
#include <concepts>

enum class setting_format { simple, percent };

class Setting
{
public:
    Setting(setting_format format = setting_format::simple)
        : m_format(format) {}

    virtual ~Setting() {}

    virtual void resetToDefault() = 0;

    virtual setting_format getFormat() const { return m_format; }

    virtual std::string toString() const = 0;

protected:
    setting_format m_format;
};

template <class T>
class ObservableSetting : public Setting
{
public:
    ObservableSetting(T value, setting_format format)
        : Setting(format), m_initial(value), m_value(value) {
    }

    delegates::observer_handler observe(delegates::consumer<T> callback, bool call_on_start = false)
    {
        const int id = m_next_id++;
        m_observers.emplace(id, callback);
        if (call_on_start)
        {
            callback(m_value);
        }


        return std::shared_ptr<int>(new int(id),
            [this](int* v_id)
            {
                m_observers.erase(*v_id);
                delete v_id;
            });
    }

    const T& get() const { return m_value; }
    const T& getDefault() const { return m_initial; }
    T& operator*() { return m_value; }

    void notify(T value)
    {
        for (auto& entry : m_observers)
        {
            entry.second(value);
        }
    }

    void set(T value)
    {
        if (value == this->m_value)
        {
            return;
        }
        this->m_value = value;
        notify(value);
    }

    virtual void resetToDefault() override { set(m_initial); }

private:
    int m_next_id = 1;
    std::unordered_map<int, delegates::consumer<T>> m_observers;

protected:
    T m_initial;
    T m_value;
};

class NumberSetting : public ObservableSetting<dl_number_t>
{
public:
    NumberSetting(dl_number_t value, 
        dl_number_t min = std::numeric_limits<dl_number_t>::min(),
        dl_number_t max = std::numeric_limits<dl_number_t>::max(), 
        setting_format format = setting_format::simple)
        : ObservableSetting(value, format), m_min(min), m_max(max) 
    {
    }

    dl_number_t& operator*() { return m_value; }

    dl_number_t get() const { return m_value; }

    dl_number_t getMin() const { return m_min; }

    dl_number_t getMax() const { return m_max; }

    dl_number_t getT() const { return (m_value - m_min) / (m_max - m_min); }

    virtual std::string toString() const override
    {
        switch (getFormat())
        {
        case setting_format::simple:
            return std::to_string(m_value);
        case setting_format::percent:
            return std::to_string(static_cast<dl_integer_t>(std::round(m_value * 100))) + "%";
        default:
            return "invalid number format";
        }
    }

    static inline NumberSetting createPercent(dl_number_t def)
    {
        return NumberSetting(def, 0.0, 1.0, setting_format::percent);
    }

protected:
    dl_number_t m_min;
    dl_number_t m_max;
};

class IntegerSetting : public ObservableSetting<dl_integer_t>
{
public:
    IntegerSetting(dl_integer_t value, dl_integer_t min = std::numeric_limits<dl_integer_t>::min(),
        dl_integer_t max = std::numeric_limits<dl_integer_t>::max(), setting_format format = setting_format::simple)
        : ObservableSetting(value, format), m_min(min), m_max(max) {}

    dl_integer_t getMin() const { return m_min; }

    dl_integer_t getMax() const { return m_max; }

    dl_integer_t getT() const { return (m_value - m_min) / (m_max - m_min); }

    virtual std::string toString() const override
    {
        switch (getFormat())
        {
            case setting_format::simple:
                return std::to_string(m_value);
            case setting_format::percent:
                return std::to_string(m_value) + "%";
            default:
                return "invalid format";
        }
    }

protected:
    dl_integer_t m_min;
    dl_integer_t m_max;
};

class FlagSetting : public ObservableSetting<bool> 
{
public:
    FlagSetting(bool value, setting_format format = setting_format::simple)
        : ObservableSetting(value, format) {}

    void toggle() { set(!get()); }

    virtual std::string toString() const override
    {
        switch (getFormat()) 
        {
            case setting_format::simple:
                return m_value ? "true" : "false";
            default:
                return "invalid format";
        }
    }
};

template<class STRINGTYPE>
class StringSetting : public ObservableSetting<STRINGTYPE> 
{
public:
    StringSetting(STRINGTYPE value, setting_format format = setting_format::simple)
        : ObservableSetting<STRINGTYPE>(value, format) 
    {}

    virtual std::string toString() const override 
    { 
        std::string ret = std::string();
        for (const auto& symb : this->m_value)
        {
            ret += static_cast<char>(symb);
        }
        return ret;
    }
};

using u32StringSetting = StringSetting<std::u32string>;
using u16StringSetting = StringSetting<std::u16string>;
using CharStringSetting = StringSetting<std::string>;

#endif // SETTINGS_SETTINGS_HPP
