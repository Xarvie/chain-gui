//
// Created by xarvie on 2020/10/7.
//
#pragma once
#ifndef VFS_FNV1A_H
#define VFS_FNV1A_H
#include <cstring>
#include <stdint.h>
#include <string>
#include <type_traits>

#ifdef _MSC_VER
#pragma warning(disable:4307)
#endif

template <typename T, T OffsetBasis, T Prime>
class fnv_basic {
    constexpr T fnv_impl(const uint8_t* data, size_t count, uint32_t index, T hash) const
    {
        return index < count ? fnv_impl(data, count, index + 1, (hash ^ data[index]) * Prime) : hash;
    }

    constexpr T fnv_impl(const char* data, size_t count, uint32_t index, T hash) const
    {
        return index < count ? fnv_impl(data, count, index + 1, (hash ^ data[index]) * Prime) : hash;
    }

public:
    constexpr fnv_basic() {}

    constexpr T operator()(const uint8_t* buf, size_t count) const
    {
        return fnv_impl(buf, count, 0, OffsetBasis);
    }

    constexpr T operator()(const char* str, size_t len) const
    {
        return fnv_impl(str, len, 0, OffsetBasis);
    }

    // byte array
    template <size_t N>
    constexpr T operator()(const uint8_t(&buf)[N]) const
    {
        return operator()(buf, N);
    }


    template <size_t N>
    constexpr T operator()(const char(&str)[N]) const
    {
        return operator()(str, N - 1);
    }

    template <typename U>
    inline typename std::enable_if<std::is_convertible<U, const char*>::value, T>::type operator()(U str) const
    {
        return operator()(str, strlen(str));
    }


    template <template <class> class Traits, template <class> class Alloc>
    inline T operator()(const std::basic_string<char, Traits<char>, Alloc<char>>& str) const
    {
        return operator()(str.c_str(), str.length());
    }
};

constexpr fnv_basic<uint32_t, 2166136261U, 16777619U> fnv32;
constexpr fnv_basic<uint64_t, 14695981039346656037ULL, 1099511628211ULL> fnv64;
#endif
