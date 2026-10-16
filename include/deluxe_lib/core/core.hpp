#ifndef CORE_CORE_HPP
#define CORE_CORE_HPP

#include <deluxe_lib/deluxe_lib_export.hpp>

#include <stdint.h>
#include <memory>
#include <functional>
#include <string>

#include "deluxe_lib/utility/meta/MetaMacros.hpp"

#define NOMINMAX

#define CORE_VERSION "0.1.0"

// #define WARN warning
// #define ERROR error
// #define INFO info
// #define DEBUG debug

#define ESP_STR(x) #x
#define XSTR(x) ESP_STR(x)

#define TIME_NS(time) (time)
#define TIME_MS(time) (TIME_NS(time) * 10)
#define TIME_S(time) (TIME_MS(time) * 100)
#define TIME_M(time) (TIME_S(time)  * 60)

template<class T>
using callback = std::function<T>;

using void_callback = callback<void(void)>;

using dl_number_t = double;
using dl_integer_t = int32_t;

namespace delegates
{
	using runnable = std::function<void()>;
	template<class T> using supplier = std::function<T()>;
	template<class T> using consumer = std::function<void(T)>;

	using key_callback = std::function<bool()>;

	// data sources
	using u32stringsupplier = std::function<std::u32string()>;
	using doublesupplier = std::function<double()>;
	using boolsupplier = std::function<bool()>;
	//using vec2supplier = std::function<glm::vec2()>;
	using key_handler = std::function<bool(int)>;

	using stringconsumer = std::function<void(const std::string&)>;
	using u32stringconsumer = std::function<void(const std::u32string&)>;
	using doubleconsumer = std::function<void(double)>;
	using boolconsumer = std::function<void(bool)>;
	using int_array_consumer = std::function<void(const int[], size_t)>;
	using u32stringchecker = std::function<bool(const std::u32string&)>;

	using observer_handler = std::shared_ptr<int>;
}

namespace container
{
    template <typename C, typename F>
    auto apply(C&& container, F&& func)
    {
        using std::begin;
        using std::end;

        using E = std::decay_t<decltype(std::forward<F>(func)(
            *begin(std::forward<C>(container))))>;

        std::vector<E> result;
        auto first = begin(std::forward<C>(container));
        auto last = end(std::forward<C>(container));

        result.reserve(std::distance(first, last));
        for (; first != last; ++first) {
            result.push_back(std::forward<F>(func)(*first));
        }
        return result;
    }
} // namespace container

#endif // CORE_CORE_HPP
