#pragma once

#include <stdexcept>

#define ASERT_NOT_NULL(value) \
        if(value == nullptr) { \
            throw std::invalid_argument("##value cannot be null");}

#define API_VERSION 0

#define ADD_GETTER(type, name) \
    protected: type _##name; \
    public: const type& name() { return _##name; } \
    void set
