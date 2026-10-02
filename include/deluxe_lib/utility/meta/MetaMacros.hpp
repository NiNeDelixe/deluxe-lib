#pragma once

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

#define DL_DISABLE_CONSTRUCTOR(Class) \
private: \
    Class() = default; \
public: \
    virtual ~Class() = default; \
private:

#define DL_DECLARE_CLASS(Class) \
    DL_SIMPLE_DECLARE_CLASS(Class) \
    DL_DISABLE_CONSTRUCTOR(Class)

#define DECLARE_PTR(Class) using Ptr = std::shared_ptr<Class>;
#define DECLARE_WPTR(Class) using WeakPtr = std::weak_ptr<Class>;

#define GETTER(type, field, Name) const type& get##Name() const { return this->field; }  
#define SETTER(type, field, Name) void set##Name(const type& value) { this->field = value; } 

#define SSETTER(type, field, Name) void Name(const type& value) { this->field = value; }  
#define SGETTER(type, field, Name) const type& Name() const { return this->field; }  

#define GETTER_VAL(type, field, Name) type get##Name() const { return this->field; }  
#define SH_GETTER_VAL(type, field, Name) type Name() const { return this->field; }  

#define GETTER_SETTER(type, field, Name) GETTER(type, field, Name) SETTER(type, field, Name)
#define SH_GETTER_SETTER(type, field, Name) SGETTER(type, field, Name) SSETTER(type, field, Name)

#define DECLARE_META_FIELD(type, field) type field; public: auto& set_##field(const type& value) { this->field = value; return *this; }

#define DECLARE_FIELD_WITH_PROPERIES(...) GET_MACRO(__VA_ARGS__, OPT_DECLARE_FIELD_WITH_PROPERIES, DEF_FALSE_DECLARE_FIELD_WITH_PROPERIES)(__VA_ARGS__)

#define GET_MACRO(_1,_2,_3,NAME,...) NAME
#define DEF_FALSE_DECLARE_FIELD_WITH_PROPERIES(type, field) OPT_DECLARE_FIELD_WITH_PROPERIES(type, field, false)
#define OPT_DECLARE_FIELD_WITH_PROPERIES(type, field, add_memeber_prefix) IMPL_DECLARE_FIELD_WITH_PROPERIES(type, field, add_memeber_prefix)
#define IMPL_DECLARE_FIELD_WITH_PROPERIES(type, field, add_memeber_prefix) IMPL_DECLARE_FIELD_WITH_PROPERIES_##add_memeber_prefix(type, field)
#define IMPL_DECLARE_FIELD_WITH_PROPERIES_true(type, field) type m_##field; public: GET_SET_ERS(type, m_##field, _##field)
#define IMPL_DECLARE_FIELD_WITH_PROPERIES_false(type, field) type field; public: GET_SET_ERS(type, field, _##field)