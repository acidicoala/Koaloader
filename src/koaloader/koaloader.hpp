#pragma once

#include <build_config.h>

#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/comma_if.hpp>

#include <Windows.h>

namespace koaloader {

    extern HMODULE original_module;

    template<typename T>
    T FnCast(FARPROC fnToCast, T) {
        return (T) fnToCast;
    }

    void init(HMODULE module);

    void shutdown();
}

#define WRAP_declare_param(r, data, i, paramType) BOOST_PP_COMMA_IF(i) paramType _ ## i
#define WRAP_forward_param(r, data, i, paramType) BOOST_PP_COMMA_IF(i) _ ## i

#ifdef _WIN64
#define PREFIX ""
#else
#define PREFIX "_"
#endif

#define ARGS(...) __VA_ARGS__

#define FUNC_IMPL(TYPE, NAME, ...) \
extern "C" BOOL __cdecl __##NAME(BOOST_PP_SEQ_FOR_EACH_I(WRAP_declare_param, ~, BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__)))) { \
    __pragma(comment(linker, "/EXPORT:"#NAME"=__" PREFIX #NAME)) \
    static auto func = FnCast(GetProcAddress(original_module, #NAME), __##NAME); \
    return func(BOOST_PP_SEQ_FOR_EACH_I(WRAP_forward_param, ~, BOOST_PP_TUPLE_TO_SEQ((__VA_ARGS__)))); \
}
