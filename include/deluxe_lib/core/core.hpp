#ifndef CORE_CORE_HPP_
#define CORE_CORE_HPP_

#include <deluxe_lib/deluxe_lib_export.hpp>

#include <stdint.h>
#include <functional>

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

#define DL_DELETE_COPY(Class) \
Class(const Class&) = delete; \
void operator=(const Class&) = delete; \
Class(Class&&) = delete; \
Class& operator=(Class&&) = delete;

#define DL_SIMPLE_DECLARE_CLASS(Class) \
public: \
    DL_DELETE_COPY(Class) \
    static Class* getPtr() { static Class instance; return &instance; } \
    static Class& getInstance() { return *getPtr(); } \
private: 

#define DL_DECLARE_CLASS(Class) \
    DL_SIMPLE_DECLARE_CLASS(Class) \
    Class() = default; \
public: \
    virtual ~Class() = default; \
private:


#endif // CORE_CORE_HPP_
